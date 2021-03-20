#define DEBUG_MODE
#include "seudo_speaker.h"
static unsigned char* buffer;
static int curx=0,cury=0;
static int row = 10,col = 20;
static struct termios orig_termios;
static int event_occur = 0;
static pthread_mutex_t lock;
static char* prefix;
static char* save_str = "(sof $s)\n($data)";
void disableRawMode(int fd){
	tcsetattr(fd,TCSAFLUSH,&orig_termios);
}
int enableRawMode(int fd, struct termios *orig){
	struct termios raw;
	tcgetattr(fd,orig);
	raw = *orig;
	/*
	//raw.c_iflag &= ~(ICRNL | INLCR | IXON);
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	//raw.c_lflag &= ~ECHOK;
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	*/
	cfmakeraw(&raw);
	tcsetattr(fd,TCSANOW,&raw);
	//raw.c_iflag |=BRKINT;
	//raw.c_lflag |=ISIG;
	//tcsetattr(fd,TCSAFLUSH, &raw);
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
	//debug_print(curx);
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
		//if(curx==1) printf("test%d\n",message_size);
		if(message_size>0) {
			//fflush(stdout);
			//fflush(stdin);
			//printf(messages[0].talk);
			//printf("\n");
			event_occur =1;
			char* bgra_text = messages[0].talk;

			bgra_text++;
			if(*bgra_text=='s'){
				_parser parser;
        			parser.index = 0;
        			parser.prev_instruction = null;
        			parse_instruction(save_str,&parser);
				_pl* sizepl = &(_pl){"primitive","0"};
				_pl* datapl = &(_pl){"primitive",(char*)buffer};
				PLreplace(parser.instruction_array[1],1,datapl);
				if(parser.instruction_array[1]->array[0]==datapl)	{
					printf("same");
				}
				printf("\n");
				char* str;
				PLstr(parser.instruction_array[1],str);
				char* its = i_to_s(str_len(str));
				sizepl->data = its;
				PLreplace(parser.instruction_array[0],2,sizepl);
				char* header;
				PLstr(parser.instruction_array[0],header);
				char* tmp = str_sum(header,"\n");
				char *sstr = str_sum(tmp,str);
				//printf(sstr);
				//printf("\n");
				//fflush(stdout);
				info(sstr,"event_manager");
				free(str);
				free(its);
				free(sstr);
				continue;
			}
			bgra_text++;
			bgra_text++;
			bgra_text++;
			bgra_text++;
			bgra_text++;
			unsigned char* bgra = (unsigned char*)bgra_text;
			//buffer = bgra;	
			//printf("\033[2J");
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

void play(){
	char tmp_buf[100];
	for(;cury<row;cury++){
		curx = 0;
			for(;curx<col;curx++){
				int color = 30;
				if(buffer[(cury*col*4)+(4*curx)]>128){
					color += 4;
				}
				if(buffer[(cury*col*4)+(4*curx)+1]>128){
					color += 2;
				}
				if(buffer[(cury*col*4)+(4*curx)+2]>128){
					color++;
				}
				if(buffer[(cury*col*4)+(4*curx)+3]>128){
					char* its = i_to_s(color);
					char* ss = str_sum("afplay ",prefix);
					char* ss2 = str_sum(ss,its);
					char* str = str_sum(ss2,".wav");
					FILE * pipe = popen(str,"r");
					if(!pipe) {
						return;
					}
					free(its);
					free(ss);
					free(ss2);
					free(str);
				}
				event_occur = 1;
				while(event_occur);
				sleep(1);
			}
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
			case 'p':
				play();
				break;
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
				buffer[(cury*col*4)+(curx*4)] = 1;
				buffer[(cury*col*4)+(curx*4)+1] = 1;
				buffer[(cury*col*4)+(curx*4)+2] = 1;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case 'l':
				buffer[(cury*col*4)+(curx*4)] = 1;
				buffer[(cury*col*4)+(curx*4)+1] = 1;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case ';':
				buffer[(cury*col*4)+(curx*4)] = 1;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 1;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case '\'':
				buffer[(cury*col*4)+(curx*4)] = 1;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case 'm':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 1;
				buffer[(cury*col*4)+(curx*4)+2] = 1;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case ',':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 1;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case '.':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 1;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case '/':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
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
        signal(SIGINT,sighandler);
	signal(SIGIO,sighandler);
	init_file_comm("seudo_speaker",1);
        pthread_t t;
        pthread_create(&t,NULL,accept_friend,NULL);
	sleep(2);
	terminal_mode();

        return 0;
}

