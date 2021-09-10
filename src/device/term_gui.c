#define DEBUG_MODE
#include "term_gui.h"
static unsigned char* buffer;
static int curx=0,cury=0;
static int row = 20,col = 60;
static struct termios orig_termios;
static int event_occur = 0;
static pthread_mutex_t lock;
static char* prefix;
static char* save_str = "(sof $s)\n($data)";
static unsigned char black[4] = {1,1,1,255};
static unsigned char red[4] = {1,1,255,255};
static unsigned char green[4] = {1,255,1,255};
static unsigned char yellow[4] = {1,255,255,255};
static unsigned char purple[4] = {255,1,1,255};
static unsigned char pink[4] = {255,1,255,255};
static unsigned char blue[4] = {255,255,1,255};
static unsigned char white[4] = {255,255,255,255};
void disableRawMode(int fd){
	tcsetattr(fd,TCSAFLUSH,&orig_termios);
}
int enableRawMode(int fd, struct termios *orig){
	struct termios raw;
	tcgetattr(fd,orig);
	raw = *orig;
	cfmakeraw(&raw);
	tcsetattr(fd,TCSANOW,&raw);
}
void* keyboard_interrupt(void*);
void terminal_mode(){
	enableRawMode(STDOUT_FILENO,&orig_termios);
	
	int alloc_size = row*col*4;
	buffer = (char*)calloc(alloc_size,1);
	for(int i=0;i<alloc_size;i++){
		buffer[i] = 1;
	}
	// size
	/*struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ,&ws);
	row = ws.ws_row;
	col = ws.ws_col;
	i*/
	// cursor
	/*
	unsigned short curx=0, cury=0;
	write(STDOUT_FILENO, "\033[6n", 4);
	read(STDIN_FILENO, null,2);
	for(int i=0;i<100;i++){
		char c;
		read(STDIN_FILENO,&c,1);
		int r = c_to_i(c);
		if(r!=-1){
			curx = curx*10 + r;	
		}else if(c=='R') break;
		else if(c==';'){
			//printf("esc : %2d\n",i);	
			cury = curx;
			curx = 0;
		}//else debug_print("error\n");
	}
	if(curx!=0) write(STDOUT_FILENO,"\r\n",2);
	//write(STDOUT_FILENO, '\e[31m',1);
	//debug_print(sizeof("\x1b[1;31m\u2588\u2588"));
	//
	*/
	//TODO: cursor init location save
	printf("\r\n\033[2J");
	printf("\033[0;0H");
	for(int i=0;i<row;i++){

		for(int j=0;j<col;j++){
			//write(STDOUT_FILENO, "\x1b[1;32m\u2588", 11);
			printf("\x1b[32m\u2588");
		}

		if(i<row-1) printf("\r\n");
	}
	pthread_t t2;
	pthread_create(&t2,NULL,keyboard_interrupt,NULL);
	//TODO : while loop ->event driven
	while(1){
		sleep(0.4);
		if(event_occur){
			printf("\033[0;0H");
			for(int i=0;i<row;i++){
				for(int j=0;j<col;j++){
					int color = 30;
					char* str;
					if(buffer[(i*col*4)+(4*j)]>128){
						color += 4;
					}	
					if(buffer[(i*col*4)+(4*j)+1]>128){
						color += 2;
					}
					if(buffer[(i*col*4)+(4*j)+2]>128){
						color++;
					}
				
					if((curx==j)&&(cury==i)) {
						char* tmp = i_to_s(color);
						char* ss = str_sum("\x1b[21;22;2;",tmp);
						str = str_sum(ss,"m\u2588\x1b[21;22;1m");
						free(ss);
						free(tmp);
					}
					else{
						char* tmp = i_to_s(color);
						char* ss = str_sum("\x1b[",tmp);
						str = str_sum(ss,"m\u2588");
						free(ss);
						free(tmp);
					}
					printf(str);
					free(str);
				}
			if(i<row-1) printf("\r\n");
			}
			event_occur = 0;
		}
		_message messages[1];
       		int message_size = clip(&messages,1);
		if(message_size>0) {
			event_occur =1;
			char* bgra_text = messages[0].talk;

			// ui thread가 느려지면 안되기 때문에 엄격한 형식주의가 필요함. 파서로 동작하지 않고 크기와 인덱스를 엄격히 지키는 구조체로 동작해야함.	
			// TODO : 크기 체크 후 안 맞을 시 실패 메시지 보내게 하기.

			int is_save = (bgra_text[1]=='s')&&(bgra_text[2]=='a')&&(bgra_text[3]=='v')&&(bgra_text[4]=='e');
			if(is_save){
				_parser parser;
        			parser.index = 0;
        			parser.prev_instruction = null;
        			parse_instruction(save_str,&parser);
				_pl* sizepl = &(_pl){"primitive","0"};
				_pl* datapl = &(_pl){"primitive",(char*)buffer};
				pl_replace(parser.instruction_array[1],1,datapl);
				if(parser.instruction_array[1]->array[0]==datapl)	{
					printf("same");
				}
				printf("\n");
				char* str;
				str = pl_str(parser.instruction_array[1]);
				char* its = i_to_s(str_len(str));
				sizepl->data = its;
				pl_replace(parser.instruction_array[0],2,sizepl);
				char* header;
				header = pl_str(parser.instruction_array[0]);
				char* tmp = str_sum(header,"\n");
				char *sstr = str_sum(tmp,str);
				info(sstr,"event_manager");
				free(str);
				free(its);
				free(sstr);
				continue;
			}
			int is_load = (bgra_text[1]=='l')&&(bgra_text[2]=='o')&&(bgra_text[3]=='a')&&(bgra_text[4]=='d');
			if(is_load){
				unsigned char* bgra = (unsigned char*)&bgra_text[6];
				for(int i=0;i<row;i++){
					for(int j=0;j<col;j++){
						if(bgra[(i*col*4)+4*j+3]>128){
							buffer[(i*col*4)+(4*j)] = bgra[(i*col*4)+(4*j)];
							buffer[(i*col*4)+(4*j)+1] = bgra[(i*col*4)+(4*j)+1];
							buffer[(i*col*4)+(4*j)+2] = bgra[(i*col*4)+(4*j)+2];
							buffer[(i*col*4)+(4*j)+3] = bgra[(i*col*4)+(4*j)+3];
						}

					}
				}
			}
		}
	}
	return;
}

void sighandler(int signum){
	//printf("%d",signum);
	switch(signum){
		case SIGINT:
			disableRawMode(STDIN_FILENO);
			exit(0);
		case SIGIO:
			break;
		default:
			break;
	}
	return;
}

void* keyboard_interrupt(void* args){
//	pthread_mutex_init(&lock,NULL);
	while(1){
		char c;
		ssize_t rret = read(STDIN_FILENO,&c,1);
		if(rret<1) printf("reading input error\n");
		if((c!='p')&&(c!='q')) event_occur =1;

		switch(c){
			case 'q':
			case 'Q':
				disableRawMode(STDIN_FILENO);
				exit(0);
				break;
			case 's':
				cury--;
				break;
			case 'x':
				cury++;
				break;
			case 'z':
				curx--;
				break;
			case 'c':
				curx++;
				break;
			case 'k':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],black,sizeof(black));
				break;
			case 'l':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],red,sizeof(red));
				break;
			case ';':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],green,sizeof(green));
				break;
			case '\'':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],yellow,sizeof(yellow));
				break;
			case 'm':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],purple,sizeof(purple));
				break;
			case ',':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],pink,sizeof(pink));
				break;
			case '.':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],blue,sizeof(blue));
				break;
			case '/':
				mem_cpy(&buffer[(cury*col*4)+(curx*4)],white,sizeof(white));
				break;
		
		}
	}
	return NULL;
}

int main(int argc, char **argv){
	char tmp[100];
	if(getcwd(tmp,sizeof(tmp))!=NULL){
		prefix = str_sum(tmp,"/data/sound_files/");
	}
	else{
		printf("cwd error\n");
		return 0;
	}
	
       /* if(argc<2){
        printf("input file missed.\n");
        return 0;
        }
        char *contents = open_file(argv[1]);

        _parser parser;
        parser.index = 0;
        parser.prev_instruction = null;
        parse_instruction(contents,&parser);
	*/
        // void *memory = (void*)malloc(sizeof(_sci64)*1000);
        // bytecode_gen64(&parser,memory);
        //
        //signal(SIGINT,sighandler);
	signal(SIGIO,sighandler);
	init_file_comm("term_gui",1);
        pthread_t t;
        pthread_create(&t,NULL,accept_friend,NULL);
	sleep(2);
	terminal_mode();

        return 0;
}

