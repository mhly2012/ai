#define DEBUG_MODE
#include "seudo_monitor.h"
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
	//tcsetattr(fd,TCSAFLUSH, &raw);
}
void* keyboard_interrupt(void*);
void terminal_mode(){
	enableRawMode(STDOUT_FILENO,&orig_termios);
	
	buffer = (char*)calloc(row*col*4,1);
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
		sleep(0.3);
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
				//if(buffer[(i*ws.ws_col*4)+(4*j)+3]>128){
					//str = str_sum(str_sum("\x1b[1;",i_to_s(color)),"m\u2588");
					//write(STDOUT_FILENO,str, 11);
				//}
					if((curx==j)&&(cury==i)) {
						str = str_sum(str_sum("\x1b[21;22;1;",i_to_s(color)),"m\u2588\x1b[21;22;2m");
					}
					else{
						str = str_sum(str_sum("\x1b[",i_to_s(color)),"m\u2588");
						//str = str_sum(str_sum("\x1b[2;",i_to_s(color)),"m\u2588");
					}
					printf(str);
				}
			if(i<row-1) printf("\r\n");
			}
			event_occur = 0;
		}
		
		_message messages[1];
       		int message_size = clip(&messages,1);
		if(message_size>0) {
			event_occur =1;
			char* bgra = messages[0].talk;
			bgra++;
			if(*bgra=='s'){
				_parser parser;
        			parser.index = 0;
        			parser.prev_instruction = null;
        			parse_instruction(save_str,&parser);
				_pl* sizepl = &(_pl){"primitive","0"};
				_pl* datapl = &(_pl){"primitive",buffer};
				PLreplace(parser.instruction_array[1],1,datapl);
				char* str = "";
				PLstr(parser.instruction_array[1],str);
				sizepl->data = i_to_s(str_len(str));
				char* header = "";
				PLstr(parser.instruction_array[0],header);
				str = str_sum(str_sum(header,"\n"),str);
				info(str,"event_manager");
				continue;
			}
			//buffer = bgra;	
			//printf("\033[2J");
			for(int i=0;i<row;i++){
				for(int j=0;j<col;j++){
					/*
					int color = 30;
					if(bgra[(i*ws.ws_col)+4*j]>128){
						color += 4;
					}
					if(bgra[(i*ws.ws_col)+4*j+1]>128){
						color += 2;
					}
					if(bgra[(i*ws.ws_col)+4*j+2]>128){
						color++;
					}
					*/
					if(bgra[(i*col*4)+4*j+3]>128){
					/*	char* str = str_sum(str_sum("\x1b[1;",i_to_s(color)),"m\u2588");
						//write(STDOUT_FILENO,str, 11);
						printf(str);
					}
					else{*/
						buffer[(i*col*4)+(4*j)] = bgra[(i*col*4)+(4*j)];
						buffer[(i*col*4)+(4*j)+1] = bgra[(i*col*4)+(4*j)+1];
						buffer[(i*col*4)+(4*j)+2] = bgra[(i*col*4)+(4*j)+2];
						buffer[(i*col*4)+(4*j)+3] = bgra[(i*col*4)+(4*j)+3];
					}

				}
				//printf("\r\n");
			}	
		}
	}
	return;
}

void sighandler(int signum){
	printf("%d",signum);
	switch(signum){
		case SIGINT:
			exit(0);
		default:
			break;
	}
	return;
}

void* keyboard_interrupt(void* args){
//	pthread_mutex_init(&lock,NULL);
	while(1){
		char c;
		read(STDIN_FILENO,&c,1);
		if(c!='q') event_occur =1;
		switch(c){
			case 'q':
			case 'Q':
				disableRawMode(STDIN_FILENO);
				exit(0);
				break;
			case 's':
				cury --;
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
				buffer[(cury*col*4)+(curx*4)] = 0;
				buffer[(cury*col*4)+(curx*4)+1] = 0;
				buffer[(cury*col*4)+(curx*4)+2] = 0;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case 'l':
				buffer[(cury*col*4)+(curx*4)] = 0;
				buffer[(cury*col*4)+(curx*4)+1] = 0;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case ';':
				buffer[(cury*col*4)+(curx*4)] = 0;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 0;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case '\'':
				buffer[(cury*col*4)+(curx*4)] = 0;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case 'm':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 0;
				buffer[(cury*col*4)+(curx*4)+2] = 0;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case ',':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 0;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case '.':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 0;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case '/':
				buffer[(cury*col*4)+(curx*4)] = 255;
				buffer[(cury*col*4)+(curx*4)+1] = 255;
				buffer[(cury*col*4)+(curx*4)+2] = 255;
				buffer[(cury*col*4)+(curx*4)+3] = 255;
				break;
			case 'K':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
			case 'L':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
			case ':':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
			case '\"':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
			case 'M':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
			case '<':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
			case '>':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
			case '?':
				buffer[cury*col*4+curx*4] = 0;
				buffer[cury*col*4+curx*4+1] = 0;
				buffer[cury*col*4+curx*4+2] = 0;
				buffer[cury*col*4+curx*4+3] = 255;
				break;
		
		}
	}
	return NULL;
}

int main(int argc, char **argv){
	char tmp[100];
	if(getcwd(tmp,sizeof(tmp))!=NULL){
		prefix = str_sum(tmp,"/../tmp/sound_files/");
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
	init_file_comm("seudo_speaker",1);
        pthread_t t;
        pthread_create(&t,NULL,accept_friend,NULL);
	sleep(2);
	terminal_mode();

        return 0;
}

