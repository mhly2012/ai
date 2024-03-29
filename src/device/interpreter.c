#define DEBUG_MODE
#include "picolisp.h"
#include "util/str.h"
#include "comm/file_comm.h"
#include <termios.h>

static int monitor_w = 60;
static int monitor_h = 20;
static unsigned char state_flag = 0; // 0 : no drawing, 1 : drawing

static pthread_cond_t draw_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t draw_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct BGMHeader{
	char type;
	int width;
	int height;
}_bgm_header;

void save_data(const char* name,const char* data){
	// data 는 ($data) 의 형태로 인덱스를 1더해서 저장
	data++;
	FILE* file;
	char* tmp = str_sum("./data/",name);
	file =	fopen(tmp,"wb");
	if(file==NULL) {
		free(tmp);
		return ;
	}
	// 헤더 추가
	_bgm_header head = {1,monitor_w,monitor_h};
	fwrite(&head,sizeof(head),1,file);
	fwrite(data,monitor_w*monitor_h*4,1,file);
	fclose(file);
	free(tmp);
	return;
}



typedef struct BGM{
	_bgm_header header;
	char* data;
} _bgm;

char* load_data(const char* name){
	FILE* file;
	char* tmp = str_sum("./data/",name);
	file = fopen(tmp,"rb");
	if(file==NULL) {
		printf("load file fail\n");
		free(tmp);
		return NULL;
	}
	_bgm_header head;
	int hsize = fread(&head,sizeof(head),1,file);
	char* data = (char*)malloc((head.width*head.height*4)+1);
	int size = fread(data,1,head.width*head.height*4,file);
	fclose(file);
	free(tmp);
	data[size] = '\0';
	return data;
}

static pthread_mutex_t lock;

//static char* monitor_str = "(sof 43)\n(monitor 20 10)";
static char* load_str = "(sof $s)\n(load $data)";
static char* save_str = "(sof $s)\n(save $data)";
static char* clear_str = "(sof $s)\n(clear)";

void save(char* name){
	_parser parser;
	parser.index =0;
	parser.prev_instruction = null;
	parse_instruction(save_str, &parser);
	_pl* sizepl = &(_pl){"primitive","0"};
	char* str;
	str = pl_str(parser.instruction_array[1]);
	char* its = i_to_s(str_len(str));
	sizepl->data = its;
	pl_replace(parser.instruction_array[0],2,sizepl);
	char* header;
	header = pl_str(parser.instruction_array[0]);
	char* tmp = str_sum(header,"\n");
	char* sstr = str_sum(tmp,str);
	//request_friend("term_gui");
	info(sstr,"term_gui");
	_message messages[1];
	int count =0;
	clip_blocking("term_gui",&messages);
	//remove_friend("term_gui");
	if(name==NULL) {
		save_data("tmp.bgm",messages[0].talk);
	}
	else {
		printf("%s\n",name);
		char *fname = str_sum(name,".bgm");
		save_data(fname,messages[0].talk);
		free(fname);
	}
	free(tmp);
	free(str);
	free(sstr);
	free(its);
}

void load(char* name){
	char *fname = str_sum(name,".bgm");
	char* bgm_data = load_data(fname);
	free(fname);
	if(bgm_data==NULL){
		return;
	}
	_parser parser;
	parser.index =0;
	parser.prev_instruction = null;
	parse_instruction(load_str, &parser);
	_pl* sizepl = &(_pl){"primitive","0"};
	_pl* datapl = &(_pl){"primitive",bgm_data};
	pl_replace(parser.instruction_array[1],2,datapl);
	char* str;
	str = pl_str(parser.instruction_array[1]);
	char* its = i_to_s(str_len(str));
	sizepl->data = its;
	pl_replace(parser.instruction_array[0],2,sizepl);
	char* header;
	header = pl_str(parser.instruction_array[0]);
	char* tmp = str_sum(header,"\n");
	char* sstr = str_sum(tmp,str);
	info(sstr,"term_gui");
	free(tmp);
	free(str);
	free(sstr);
	free(bgm_data);
	free(its);
	
}

// CAT BOX 

typedef struct Cat{
	int dimx;
	int dimy;
	unsigned char* data;
	int locx;
	int locy;
	int locz;
}_cat;

typedef struct NewWorld{
	int dimx;
	int dimy;
	int dimz;
	_cat* objs;
	int objs_length;
	int curx;
	int cury;
	int curz;
}_nworld;

typedef struct GameEvent{
	int type;
	int dimx;
	int dimy;
	char* data;
	int data_length;
	int locx;
	int locy;
	int locz;
}_event;

typedef struct Game{
	int dimx;
	int dimy;
	int dimz;
	_cat* objs;
	int objs_length;
	int curx;
	int cury;
	int curz;
	_event* events;
	int events_length;
} _game;

static _nworld _box;
static _game _slave;
static _cat _hand;
static _cat _avatar={0,0,NULL,0,0,0};

void save_world(const char* name){
	FILE* file;
	char* tmp2 = str_sum(name,".wld");
	char* tmp = str_sum("./data/",tmp2);
	file = fopen(tmp,"wb");
	if(file==NULL){
		free(tmp);
		return;
	}
	fwrite(&_box,sizeof(_nworld),1,file);
	fwrite(_box.objs,sizeof(_cat)*_box.objs_length,1,file);
	for(int i=0;i<_box.objs_length;i++){
		fwrite(_box.objs[i].data,_box.objs[i].dimx*_box.objs[i].dimy*4,1,file);
	}
	
	close(file);
	free(tmp);
	free(tmp2);
	
	return;
}
void load_world(const char* name){
	FILE* file;
	char* tmp2 = str_sum(name,".wld");
	char* tmp = str_sum("./data/",tmp2);
	file = fopen(tmp,"rb");
	if(file==NULL){
		printf("load file fail\n");
		free(tmp);
		return;
	}
	int bsize = fread(&_box,sizeof(_nworld),1,file);
	
	_slave.dimx = _box.dimx;
	_slave.dimy = _box.dimy;
	_slave.dimz = _box.dimz;
	_slave.objs = _box.objs;
	_slave.objs_length = _box.objs_length;
	_slave.curx = _box.curx;
	_slave.cury = _box.cury;
	_slave.curz = _box.curz;
	printf("cats count : %d\n",_box.objs_length);
	_box.objs = (_cat*)malloc((sizeof(_cat)*_box.objs_length));
	fread(_box.objs,1,sizeof(_cat)*_box.objs_length,file);
	_slave.objs = _box.objs;
	for(int i=0;i<_box.objs_length;i++){
		_box.objs[i].data = (char*)malloc(_box.objs[i].dimx*_box.objs[i].dimy*4);
		fread(_box.objs[i].data,1,_box.objs[i].dimx*_box.objs[i].dimy*4,file);
		_slave.objs[i].data = _box.objs[i].data;
	}
	fclose(file);
	free(tmp2);
	free(tmp);
	return;
}

void save_game(const char* name){
	FILE* file;
	char* tmp2 = str_sum(name,".gme");
	char* tmp = str_sum("./data/",tmp2);
	file = fopen(tmp,"wb");
	if(file==NULL){
		free(tmp);
		return;
	}
	fwrite(&_slave,sizeof(_game),1,file);
	fwrite(_slave.objs,sizeof(_cat)*_slave.objs_length,1,file);
	for(int i=0;i<_slave.objs_length;i++){
		fwrite(_slave.objs[i].data,_slave.objs[i].dimx*_slave.objs[i].dimy*4,1,file);
	}
	fwrite(_slave.events,sizeof(_event)*_slave.events_length,1,file);
	for(int i=0;i<_slave.events_length;i++){
		fwrite(_slave.events[i].data,_slave.events[i].data_length,1,file);
	}
	
	close(file);
	free(tmp);
	free(tmp2);
	
	return;

}

void load_game(const char* name){
	FILE* file;
	char* tmp2 = str_sum(name,".gme");
	char* tmp = str_sum("./data/",tmp2);
	file = fopen(tmp,"rb");
	if(file==NULL){
		printf("load file fail\n");
		free(tmp);
		return;
	}
	int bsize = fread(&_slave,sizeof(_game),1,file);
	printf("cats count : %d\n",_slave.objs_length);
	_slave.objs = (_cat*)malloc((sizeof(_cat)*_slave.objs_length));
	fread(_slave.objs,1,sizeof(_cat)*_slave.objs_length,file);
	for(int i=0;i<_slave.objs_length;i++){
		_slave.objs[i].data = (char*)malloc(_slave.objs[i].dimx*_slave.objs[i].dimy*4);
		fread(_slave.objs[i].data,1,_slave.objs[i].dimx*_slave.objs[i].dimy*4,file);
	}
	fread(_slave.events,1,sizeof(_event)*_slave.events_length,file);
	for(int i=0;i<_slave.events_length;i++){
		_slave.events[i].data = (char*)malloc(_slave.events[i].data_length);
		fread(_slave.events[i].data,1,_slave.events[i].data_length,file);
	}
	fclose(file);
	free(tmp2);
	free(tmp);
	return;

}

void set_avatar(){
	_avatar = _hand;
}

void put_event(int type,int dimx,int dimy, char* data, int data_length){
	_slave.events = (_event*)realloc(_slave.events,sizeof(_event)*(_slave.events_length+1));
	_event e = {type,dimx,dimy,data,data_length,_slave.curx,_slave.cury,_slave.curz};
	_slave.events[_slave.events_length] = e;
	_slave.events_length++;
}

void put_bgm(){
	_box.objs = (_cat*)realloc(_box.objs,sizeof(_cat)*(_box.objs_length+1));
	_hand.locx = _box.curx;
	_hand.locy = _box.cury;
	_hand.locz = _box.curz;
	_box.objs[_box.objs_length] = _hand;
	_box.objs_length++;

	_slave.objs = (_cat*)realloc(_slave.objs,sizeof(_cat)*(_slave.objs_length+1));
	_slave.objs[_slave.objs_length] = _hand;
	_slave.objs_length++;
}

void pick_bgm(){
	_parser parser;
	parser.index =0;
	parser.prev_instruction = null;
	parse_instruction(save_str, &parser);
	_pl* sizepl = &(_pl){"primitive","0"};
	char* str;
	str = pl_str(parser.instruction_array[1]);
	char* its = i_to_s(str_len(str));
	sizepl->data = its;
	pl_replace(parser.instruction_array[0],2,sizepl);
	char* header;
	header = pl_str(parser.instruction_array[0]);
	char* tmp = str_sum(header,"\n");
	char* sstr = str_sum(tmp,str);
	//request_friend("term_gui");
	info(sstr,"term_gui");
	_message messages[1];
	int count =0;
	clip_blocking("term_gui",&messages);
	//remove_friend("term_gui");
	unsigned char* data = messages[0].talk;
	_hand.dimx = monitor_w;
	_hand.dimy = monitor_h;
	// data가 ($data)의 형태로 괄호 다음부터 사용하기 위해 인덱스를 더함
	_hand.data = ++data;
	free(tmp);
	free(str);
	free(sstr);
	free(its);
}

void exec_command(char* command){
	if(str_cmp(command,"(command-list)")){
		printf("(clear)");
		printf("(save $name)");
		printf("(load $name)");
		printf("(pick)");
		printf("(put)");
		printf("(avatar)");
		printf("(keyboard)");
		printf("(save-world $name)");
		printf("(load-world $name)");
		printf("(quit)");
	}else if(str_cmp(command,"(signature)")){
		char* bgm_data = load_data("dog.bgm");
		if(bgm_data==NULL){
			return;
		}
		_parser parser;
		parser.index =0;
		parser.prev_instruction = null;
		parse_instruction(load_str, &parser);
		_pl* sizepl = &(_pl){"primitive","0"};
		_pl* datapl = &(_pl){"primitive",bgm_data};
		pl_replace(parser.instruction_array[1],2,datapl);
		char* str;
		str = pl_str(parser.instruction_array[1]);
		char* its = i_to_s(str_len(str));
		sizepl->data = its;
		pl_replace(parser.instruction_array[0],2,sizepl);
		char* header;
		header = pl_str(parser.instruction_array[0]);
		char* tmp = str_sum(header,"\n");
		char* sstr = str_sum(tmp,str);
		info(sstr,"term_gui");
		free(str);
		free(tmp);
		free(sstr);
		free(its);
		free(bgm_data);

	}else if(str_cmp(command,"(gifts)")){
		printf("not ready...some years later...:(\n");
	}else if(str_cmp(command,"(clear)")){
		_parser parser;
		parser.index =0;
		parser.prev_instruction = null;
		parse_instruction(clear_str, &parser);
		_pl* sizepl = &(_pl){"primitive","0"};
		char* str;
		str = pl_str(parser.instruction_array[1]);
		char* its = i_to_s(str_len(str));
		sizepl->data = its;
		pl_replace(parser.instruction_array[0],2,sizepl);
		char* header;
		header = pl_str(parser.instruction_array[0]);
		char* tmp = str_sum(header,"\n");
		char* sstr = str_sum(tmp,str);
		info(sstr,"term_gui");
		free(str);
		free(tmp);
		free(sstr);
		free(its);
	}else if(str_cmp(command,"(save)")){
		save(NULL);		
	}else if(str_cmp(command,"(load)")){
		char* bgm_data = load_data("tmp.bgm");
		_parser parser;
		parser.index =0;
		parser.prev_instruction = null;
		parse_instruction(load_str, &parser);
		_pl* sizepl = &(_pl){"primitive","0"};
		_pl* datapl = &(_pl){"primitive",bgm_data};
		pl_replace(parser.instruction_array[1],2,datapl);
		char* str;
		str = pl_str(parser.instruction_array[1]);
		char* its = i_to_s(str_len(str));
		sizepl->data =its;
		pl_replace(parser.instruction_array[0],2,sizepl);
		char* header;
		header = pl_str(parser.instruction_array[0]);
		char* tmp = str_sum(header,"\n");
		char* sstr = str_sum(tmp,str);
		info(sstr,"term_gui");
		free(its);
		free(bgm_data);
		free(str);
		free(sstr);
		free(tmp);
	}
	else if(str_cmp(command,"(quit)")){
		exit(0);
	}else{
		_parser parser = {0, NULL,NULL, 0};
		parse_instruction(command, &parser);
		_pl* command_pl = parser.instruction_array[0];
		_pl* name_pl = pl_get(command_pl,1);
		if(str_cmp(name_pl->data,"save")){
			_pl* arg1 = pl_get(command_pl,2);
			save(arg1->data);
		}else if(str_cmp(name_pl->data,"load")){
			_pl* arg1 = pl_get(command_pl,2);
			load(arg1->data);
		}else if(str_cmp(name_pl->data,"save-world")){
			_pl* arg1 = pl_get(command_pl,2);
			save_world(arg1->data);
		}else if(str_cmp(name_pl->data,"load-world")){
			_pl* arg1 = pl_get(command_pl,2);
			load_world(arg1->data);
			pthread_cond_signal(&draw_cond);
		}else if(str_cmp(name_pl->data,"world-size")){
			//3차원 세계모델의 크기	
			_pl* arg1 = pl_get(command_pl,2);
			_pl* arg2 = pl_get(command_pl,2);
			_pl* arg3 = pl_get(command_pl,2);
		}else if(str_cmp(name_pl->data,"put")){
			put_bgm();
			//월드 위치 지정	
		}else if(str_cmp(name_pl->data,"throw")){

		}else if(str_cmp(name_pl->data,"together")){
			//소켓 통신, openSSL
			
		}else if(str_cmp(name_pl->data,"avatar")){
			set_avatar();	
		
		}else if(str_cmp(name_pl->data,"mode")){
			//주파수(모드) 선택
		
		}else if(str_cmp(name_pl->data,"pick")){
			pick_bgm();
			//집어들기
		}else if(str_cmp(name_pl->data,"down")){
			_pl* arg1 = pl_get(command_pl,2);
			int a = _box.cury+s_to_i(arg1->data);
			int b = _box.dimy;
			int r = b ^ ((a ^ b) & -(a < b));
			_box.cury = r;
			_slave.cury = r;
		}else if(str_cmp(name_pl->data,"up")){
			_pl* arg1 = pl_get(command_pl,2);
			int a = _box.cury-s_to_i(arg1->data);
			int b = 0;
			int r = a ^ ((a ^ b) & -(a < b));
			_box.cury = r;
			_slave.cury = r;
		}else if(str_cmp(name_pl->data,"left")){
			_pl* arg1 = pl_get(command_pl,2);
			int a = _box.curx-s_to_i(arg1->data);
			int b = 0;
			int r = a ^ ((a ^ b) & -(a < b));
			_box.curx = r;
			_slave.curx = r;
		}else if(str_cmp(name_pl->data,"right")){
			_pl* arg1 = pl_get(command_pl,2);
			int a = _box.curx+s_to_i(arg1->data);
			int b = _box.dimx;
			int r = b ^ ((a ^ b) & -(a < b));
			_box.curx = r;
			_slave.curx = r;
		}else if(str_cmp(name_pl->data,"dive")){
			_box.curz++;
			_slave.curz++;
		}else if(str_cmp(name_pl->data,"undive")){
			_box.curz--;
			_slave.curz--;
		}else if(str_cmp(name_pl->data,"animate")){
			_pl* arg1 = pl_get(command_pl,2);
			_pl* arg2 = pl_get(command_pl,3);
			// 쓰레드 추가하여 애니메이션 관리
		}else if(str_cmp(name_pl->data,"start")){
			state_flag = 1;
		}else if(str_cmp(name_pl->data,"stop")){
			state_flag = 0;
		}else if(str_cmp(name_pl->data,"here")){
			printf("enter the number matched with a type of event\n");
			printf("1. play music\n");
			printf("2. speak text\n");
			printf("3. play animation\n");
			char input[2];
			char * res = fgets(input,2,stdin);
			//int n = c_to_i(input[0]);
			fflush(stdout);
			fflush(stdin);

			switch(res[0]){
				case '1':
					printf("write file name to play\n");
					
					break;
				case '2':
					printf("write text to speak\n");
					char tts[100];
					char* res = fgets(tts,100,stdin);
					res = str_sum("say ",res);
					int fp = popen(res,"r");
					put_event(2,5,5,res,str_len(res));
					break;
				case '3':
					break;
				default:
					printf("wrong number\n");
			}
			
		}else if(str_cmp(name_pl->data,"keyboard")){
			struct termios oldt, newt;
			int c;
			tcgetattr( STDIN_FILENO, &oldt );
			newt = oldt;
			newt.c_lflag &= ~( ICANON | ECHO );
			tcsetattr( STDIN_FILENO, TCSANOW, &newt );
			while((c = getchar())!=27){
				if(c == 27)   break;
				if(c=='x'){
					int a = _box.cury+1;
					int b = _box.dimy;
					int r = b ^ ((a ^ b) & -(a < b));
					_box.cury = r;
					_slave.cury = r;
				}
				if(c=='s'){
					int a = _box.cury-1;
					int b = 0;
					int r = a ^ ((a ^ b) & -(a < b));
					_box.cury = r;
					_slave.cury =r;
				}
				if(c=='z'){
					int a = _box.curx-1;
					int b = 0;
					int r = a ^ ((a ^ b) & -(a < b));
					_box.curx = r;
					_slave.curx = r;
				}
				if(c=='c'){
					int a = _box.curx+1;
					int b = _box.dimx;
					int r = b ^ ((a ^ b) & -(a < b));
					_box.curx = r;
					_slave.curx = r;
				}
				pthread_cond_signal(&draw_cond);
			}
			tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
		}else if(str_cmp(name_pl->data,"count-cat")){
			printf("%d\n",_box.objs_length);
		}else if(str_cmp(name_pl->data,"loc")){
			printf("%d-%d-%d\n",_box.curx,_box.cury,_box.curz);
		}else if(str_cmp(name_pl->data,"all-cat")){
			for(int i=0;i<_box.objs_length;i++){
				printf("%d-%d-%d\n",_box.objs[i].locx,_box.objs[i].locy,_box.objs[i].locz);
			}
		}else{
			printf("command is not exist\n");
		}
	}
	fflush(stdout);
	fflush(stdin);
}

static char* dummy_str = "(sof $size)\n(dummy)";
unsigned char arr[4000000];	
unsigned char earr[1000000];

void play_event(){
	for(int i=0;i<_slave.events_length;i++){
		int _minx = _slave.events[i].locx <= _slave.curx;
		int _maxx = _slave.events[i].locx+_slave.events[i].dimx >= _slave.curx;
		int _miny = _slave.events[i].locy <= _slave.cury;
		int _maxy = _slave.events[i].locy + _slave.events[i].dimy >= _slave.cury;
		int z = _slave.events[i].locz == _slave.curz;
		if(_minx&&_maxx&&_miny&&_maxy&&z){
			switch(_slave.events[i].type){
				case 1:
					break;
				case 2:
					popen(_slave.events[i].data,"r");
					break;
				case 3:
					break;
				default:
					break;
			}	
		}
	}
}

void* draw_box(void* args){
	mem_set(arr,255,_box.dimx*_box.dimy*_box.dimz*4);
	request_friend("term_gui");
	while(1){
		pthread_mutex_lock(&draw_lock);
		pthread_cond_wait(&draw_cond,&draw_lock);
		pthread_mutex_unlock(&draw_lock);
		
		// 고양이들을 박스에 그려넣음
		for(int i=0;i<_box.objs_length;i++){
			_cat cat = _box.objs[i];
			for(int j=0;j<cat.dimy;j++){
				for(int k=0;k<cat.dimx;k++){
					
					int dst_i = ((((cat.locz*_box.dimy)+cat.locy+j)*_box.dimx)+cat.locx+k)*4;
					int index = ((cat.dimx*j)+k)*4;
					if(cat.data[index+3]==255){
						arr[dst_i] = cat.data[index];
						arr[dst_i+1] = cat.data[index+1];
						arr[dst_i+2] = cat.data[index+2];
						arr[dst_i+3] = cat.data[index+3];
					}
				}
			}
		}
		

		// 박스 중 모니터에 그려질 공간을 따로 빼냄
		
		unsigned char *bgm_data= (unsigned char*)malloc((monitor_h*monitor_w*4));
		for(int j=0;j<monitor_h;j++){
			for(int k=0;k<monitor_w;k++){
				
				int index = ((((_box.curz*_box.dimy)+_box.cury+j)*_box.dimx)+_box.curx+k)*4;
				int dst_i = ((j*monitor_w)+k)*4;
					bgm_data[dst_i] = arr[index];
					bgm_data[dst_i+1] = arr[index+1];
					bgm_data[dst_i+2] = arr[index+2];
					bgm_data[dst_i+3] = arr[index+3];
				if((_avatar.data!=NULL)&&(_avatar.data[dst_i+3]==255)){
					bgm_data[dst_i] = _avatar.data[dst_i];
					bgm_data[dst_i+1] = _avatar.data[dst_i+1];
					bgm_data[dst_i+2] = _avatar.data[dst_i+2];
					bgm_data[dst_i+3] = _avatar.data[dst_i+3];
				}
			}
		}

		bgm_data[monitor_h*monitor_w*4] = '\0';
		_parser parser;
		parser.index =0;
		parser.prev_instruction = null;
		parse_instruction(load_str, &parser);
		_pl* sizepl = &(_pl){"primitive","0"};
		_pl* datapl = &(_pl){"primitive",bgm_data};
		pl_replace(parser.instruction_array[1],2,datapl);
		char* str;
		str = pl_str(parser.instruction_array[1]);
		char* its = i_to_s(str_len(str));
		sizepl->data = its;
		pl_replace(parser.instruction_array[0],2,sizepl);
		char* header;
		header = pl_str(parser.instruction_array[0]);
		char* tmp = str_sum(header,"\n");
		char* sstr = str_sum(tmp,str);
		//pthread_mutex_lock(&lock);
		//printf("%s\n",sstr);
		info(sstr,"term_gui");
		//pthread_mutex_unlock(&lock);
		free(tmp);
		free(str);
		free(sstr);
		free(its);
		//free(bgm_data);

		// call event
		play_event();
	}
	remove_friend("term_gui");
}

int main(){
	init_file_comm("event_manager",2);
	printf("command-list\n");
	printf("(gifts)\n(clear)\n");
	printf("(dog)\n");
	printf("(quit)\n");
	int pre=0;
	char* pre_input = (char*)malloc(1);
	pre_input[0] = '\0';
	printf(">>>");
	fflush(stdout);
	
	// ui control thread
	//pthread_mutex_init(&lock,NULL);
	_box.dimx = monitor_h+100;
	_box.dimy = monitor_w+100;
	_box.dimz = 1;
	_box.objs = (_cat*)malloc(sizeof(_cat)*0);
	_box.objs_length = 0;
	_box.curx = 0;
	_box.cury = 0;
	_box.curz = 0;
	pthread_t thr;
	pthread_create(&thr,NULL,draw_box,NULL);

	// command processing loop
	while(1){
		char input[30];
		char * res = fgets(input,30,stdin);
		if((res!=NULL)){
			if((pre==0)&&(res[0]!='(')) {
				printf("wrong command\n");
				pre=0;
				free(pre_input);
				pre_input = (char*)malloc(1);
				pre_input[0] = '\0';
				printf(">>>");
				fflush(stdout);
				continue;
			}
			int input_len = str_len(input);
			for(int i=0;i<input_len;i++){
				if(res[i]=='('){
					pre++;
				}else if(res[i]==')'){
					pre--;
				}
			}
			input[input_len-1] = '\0';
			if(pre==0) {
				char *com = str_sum(pre_input,input);
				exec_command(com);
				free(com);
				pre_input = (char*)malloc(1);
	pre_input[0] = '\0';
				printf(">>>");
				fflush(stdout);
			}
			else if(pre<0){
				printf("wrong command\n");
				pre=0;
				pre_input = (char*)malloc(1);
	pre_input[0] = '\0';
				printf(">>>");
				fflush(stdout);
			}else{
				pre_input = str_sum(pre_input,input);
			}
		}
	}
	return 0;
}
