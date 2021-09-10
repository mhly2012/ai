#define DEBUG_MODE
#include "picolisp.h"
#include "util/str.h"
#include "comm/file_comm.h"

static int monitor_w = 60;
static int monitor_h = 20;

typedef struct BGMHeader{
	char type;
	int width;
	int height;
}_bgm_header;
void save_data(const char* name,const char* data){
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

void send_instant(char* name,char* contents){
	request_friend(name);
	info(contents,name);
	remove_friend(name);
}

//static char* monitor_str = "(sof 43)\n(monitor 20 10)";
static char* load_str = "(sof $s)\n(load $data)";
static char* save_str = "(sof $s)\n(save $data)";

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
	request_friend("term_gui");
	info(sstr,"term_gui");
	_message messages[1];
	int count =0;
	count = clip_blocking(&messages,1);
	remove_friend("term_gui");
	if(count>0){
		if(name==NULL) {
			save_data("tmp.bgm",messages[0].talk);
		}
		else {
			printf("%s\n",name);
			char *fname = str_sum(name,".bgm");
			save_data(fname,messages[0].talk);
			free(fname);
		}
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
	send_instant("term_gui",sstr);
	free(tmp);
	free(str);
	free(sstr);
	free(bgm_data);
	free(its);
}

void exec_command(char* command){
	if(str_cmp(command,"(command-list)")){
		printf("(bgm)\n(cake)\n(signature)\n(gifts)\n(number)\n(clear)\n(quit)\n");	
	}else if(str_cmp(command,"(bgm)")){
		char* bgm_data = load_data("birth.bgm");
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
		send_instant("term_gui",sstr);
		free(tmp);
		free(str);
		free(sstr);
		free(bgm_data);
		free(its);
	}else if(str_cmp(command,"(cake)")){
		char* bgm_data = load_data("cake.bgm");
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
		send_instant("term_gui",sstr);
		free(tmp);
		free(str);
		free(sstr);
		free(its);
		free(bgm_data);
	}else if(str_cmp(command,"(signature)")){
		char* bgm_data = load_data("signature.bgm");
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
		send_instant("term_gui",sstr);
		free(str);
		free(tmp);
		free(sstr);
		free(its);
		free(bgm_data);

	}else if(str_cmp(command,"(dog)")){
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
		send_instant("term_gui",sstr);
		free(str);
		free(tmp);
		free(sstr);
		free(its);
		free(bgm_data);

	}else if(str_cmp(command,"(gifts)")){
		printf("not ready...some years later...:(\n");
	}else if(str_cmp(command,"(clear)")){

	// 첫번째 클리어는 화면을 일단 검은색으로 모두 칠함
	{
		//load("clear1");
		static unsigned char black[4] = {1,1,1,255};
	}
		sleep(2);
	// 두번째 클리어는 화면을 빈화면으로 바꿈
	{
		//load("clear2");
		static unsigned char black[4] = {1,1,1,0};
	}

	}else if(str_cmp(command,"(number)")){
		char* bgm_data = load_data("number.bgm");
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
		send_instant("term_gui",sstr);
		free(tmp);
		free(str);
		free(sstr);
		free(bgm_data);
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
		send_instant("term_gui",sstr);
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
		}else if(str_cmp(name_pl->data,"animate")){
			_pl* arg1 = pl_get(command_pl,2);
			_pl* arg2 = pl_get(command_pl,3);
			// 쓰레드 추가하여 애니메이션 관리
		}else{
			printf("command is not exist\n");
		}
	}
	fflush(stdout);
	fflush(stdin);
}

int main(){
	init_file_comm("event_manager",1);
	printf("command-list\n");
	printf("(gifts)\n(clear)\n");
	printf("(dog)\n");
	printf("(quit)\n");
	int pre=0;
	char* pre_input = (char*)malloc(0);
	printf(">>>");
	fflush(stdout);
	while(1){
		char input[30];
		char * res = fgets(input,30,stdin);
		if((res!=NULL)){
			if((pre==0)&&(res[0]!='(')) {
				printf("wrong command\n");
				pre=0;
				pre_input=(char*)malloc(0);
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
				free(pre_input);
				pre_input = (char*)malloc(0);
				printf(">>>");
				fflush(stdout);
			}
			else if(pre<0){
				printf("wrong command\n");
				pre=0;
				free(pre_input);
				pre_input = (char*)malloc(0);
				printf(">>>");
				fflush(stdout);
			}else{
				pre_input = str_sum(pre_input,input);
			}
		}
	}
	return 0;
}
