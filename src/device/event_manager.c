#define DEBUG_MODE
#include "picolisp.h"
#include "util/str.h"
#include "comm/file_comm.h"
typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	DWORD bfReserved2;
	DWORD bf0ffBits;
} BITMAPFILEHEADER;

#pragma pack(push, 1)
typedef struct tagBITMAPINFOHEADER{
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImpotant;
}BITMAPINFOHEADER;
#pragma pack(pop)

static int monitor_w = 60;
static int monitor_h = 20;

// 비트맵 -> raw데이터로 읽기
/*
unsigned char* load_bitmap_data(char* filename){
	FILE* file;
	BITMAPINFOHEADER bih;
	BITMAPFILEHEADER bfh;
	unsigned char* image;
	int idx =0;
	char* name = str_sum("./data/",filename);
	file =	fopen(name,"rb");
	free(name);
	if(file==NULL) return NULL;
	fread(&bfh,sizeof(BITMAPFILEHEADER),1,file);
	if(bfh.bfType!=0x4d42){
		fclose(file);
		printf("bitmap file not exist!\n");
		return NULL;
	}
	fread(&bih,sizeof(BITMAPINFOHEADER),1,file);
	if((bih.biWidth!=20)||(bih.biHeight!=10)){
		printf("bitmap size wrong! resize to 20x10\n");
		fclose(file);
		return NULL;
	}
	fseek(file,bfh.bf0ffBits,SEEK_SET);
	image = (unsigned char*)malloc(bih.biSizeImage);
	if(!image){
		free(image);
		fclose(file);
		return NULL;
	}
	fread(image,bih.biSizeImage,1,file);
	if(image==NULL){
		fclose(file);
		return NULL;
	}
	fclose(file);
	unsigned char *tmp = (unsigned char*)malloc(20*10*4);
	for(int i=0;i<200;i++){
		tmp[(4*i)] = image[3*i];
		tmp[(4*i)+1] = image[(3*i)+1];
		tmp[(4*i)+2] = image[(3*i)+2];
		tmp[(4*i)+3] = 255;
	}
	free(image);
	image = tmp;
	return image;
}
*/
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
	if(name==NULL){
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
			save_data("tmp.bgm",messages[0].talk);
		}	
		free(tmp);
		free(str);
		free(sstr);
		free(its);
		
	}else{
		
	}
}

// bitmap데이터 모니터로 전송하기 
/*
void load_bitmap(char* name){
	char* bgm_data = load_bitmap_data(name);
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
*/

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
	{
	char* bgm_data = load_data("clear1.bgm");
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
	sleep(2);
	{
	char* bgm_data = load_data("clear2.bgm");
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
	/*
	else if(str_contains(command,"(load-bitmap")>-1){
		_parser parser;
	parser.index =0;
	parser.prev_instruction = null;
	parse_instruction(command, &parser);
	_pl* pl;
	pl = pl_get(parser.instruction_array[0],2);
	int con = str_contains(pl->data,".bmp");
	if(con+4==str_len(pl->data)) load_bitmap(pl->data);
	}
	*/
	else if(str_cmp(command,"(quit)")){
		exit(0);
	}else{
		printf("not exist\n");
	}
	fflush(stdout);
	fflush(stdin);
}

int main(){
	init_file_comm("event_manager",1);
	//request_friend("seudo_speaker");
	//send(setupevent, "seudo_monitor");
	
	printf("type (command-list)\n");
	while(1){
		printf(">>>");
		fflush(stdout);
		char input[30];
		char * res = fgets(input,30,stdin);
		if(res!=NULL){
			int size = str_len(input);
			input[size-1] = '\0';
			exec_command(input);
		}
	}
	return 0;
}
