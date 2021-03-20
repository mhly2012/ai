#define  DEBUG_MODE
#include "file_comm.h"
#define LISTENQ 10
#define DISCONNECT "(disconnect)"
static _friend *friends;
static int f_cnt;
static int max_friends;
static char *my_name;
//static _message* queue;
//static int q_index = 0;
//static int max_queue;

static char* prefix;
char* parse_request(int connfd){
	ssize_t rret;
	char buf_c;
	char fbuf[100];
	char* buf = fbuf;
	size_t bufcur=0,prevcur=0;
	size_t size;
	while(1){
		while(((rret=read(connfd,&buf_c,sizeof(buf_c)))==-1)&&errno==EINTR);
		if((rret==0)||(errno==ECONNRESET)) {
			//printf("disconnect occur\n");
			//connect_flag= 0;
			return DISCONNECT;
		}
		if(errno==EAGAIN) return NULL;
		buf[bufcur] = buf_c;
		bufcur++;
		if(buf_c=='\n') {
			buf[bufcur] = null;
			_parser parser;
			parser.index = 0;
			parser.prev_instruction = null;
			parse_instruction(buf,&parser);
			_pl* pl = pl_get(parser.instruction_array[0],1);
			//printf(buf);
			//PLget(parser.instruction_array[0],1,pl);
			if(str_cmp(pl->data,"sof")){
				pl = pl_get(parser.instruction_array[0],2);
				//PLget(parser.instruction_array[0],2,pl)
				size = s_to_i(pl->data);
				size++;
				bufcur = 0;
				buf = (char*)malloc(sizeof(char)*size);
				break;
			}
			else{
				return NULL;	
			}
		}
	}
	while(1){
                while(((rret=read(connfd,buf,size))==-1)&&errno==EINTR);
		if(errno==EAGAIN) {
			printf("contents read fail\n");
			free(buf);
			return NULL;
		}
		if(rret<=0) {
                        printf("fail\n");
                        break;
                }
		prevcur = bufcur;
                bufcur += rret;
		if(bufcur==size) {
		//	printf("%d\n",size);
		//	printf(buf);
		//	printf("\n");
			return buf;
		}
	}
	free(buf);
	printf("something wrong\n");
	return NULL;
}

void* accept_friend(void* args){
	if(my_name==NULL) {
		printf("name empty");
		return NULL;
	}
	char* fname = str_sum(prefix,my_name);
	unlink(fname);
	struct sockaddr_un serveraddr;
	int connfd;
        int listenfd, optval = 1;
        listenfd = socket(PF_LOCAL, SOCK_STREAM,0);
        
	
	
if(setsockopt(listenfd, SOL_SOCKET,SO_REUSEADDR,(const void*)&optval,sizeof(int))<0) {
		printf("setsockopt fail\n");
		return NULL;
	}

        memset(&serveraddr,0,sizeof(serveraddr));
        serveraddr.sun_family=AF_LOCAL;
	mem_move(serveraddr.sun_path,fname,str_len(fname)+1);
	free(fname);
        if(bind(listenfd, (struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
		debug_print("bind fail\n");
                return NULL;
	}
        if(listen(listenfd,LISTENQ)<0) {
		debug_print("listen fail\n");
		return NULL;
	}

	while(1){
                connfd = accept(listenfd,NULL,NULL);

		if(f_cnt>=max_friends) {
			printf("fmax exceed!\n");
			write(connfd,"(sof 17)\n(error full-list)\0",27);
			close(connfd);
			continue;
		}
		//fcntl(connfd,F_SETFL, O_NONBLOCK);
		char* req = parse_request(connfd);
		if(req!=NULL){

		_parser parser;
		parser.index = 0;
		parser.prev_instruction = null;
		parse_instruction(req,&parser);
		_pl* pl;
		pl = pl_get(parser.instruction_array[0],1);
		//PLget(parser.instruction_array[0],1,pl)

		if(str_cmp(pl->data,"name")){
			pl = pl_get(parser.instruction_array[0],2);
			//PLget(parser.instruction_array[0],2,pl)
			char * name = pl->data;
			/*
			PLget(parser.instruction_array[1],1,pl)
			if(str_cmp(pl->data,"purpose")){

			}*/
			//TODO : add friend
			//fcntl(connfd,F_SETFL, O_NONBLOCK);

			friends[f_cnt].connfd = connfd;
			friends[f_cnt].name = name;
			f_cnt++;
			write(connfd,"(sof 9)\n(success)\0",18);
			//non_block_mode();
		}
		else{
			printf("no name\n");
			close(connfd);
		}
		free(req);}
		else{
			printf("req==null\n");
			close(connfd);
		}
		//close(connfd);
        }
	return NULL;
}

void info(const char* message, const char* name){
	for(int i=0;i<f_cnt;i++){
		if(str_cmp(friends[i].name,name)){
			//printf("info fd:%d\n",(friends[i].connfd));
			//printf(message);
			//printf("\n");
			write((friends[i].connfd),message,str_len(message)+1);
			
			return;
		}
	}
}

int clip_blocking(_message* messages,int length){
	int res_size = 0;
	for(int i=0;i<f_cnt;i++){
		//printf("block fd:%d\n",(friends[i].connfd));
		int opts = fcntl((friends[i].connfd),F_GETFL);
		opts &=~(O_NONBLOCK);
		fcntl((friends[i].connfd),F_SETFL, opts);
		char* req = parse_request((friends[i].connfd));
		
		while(req!=NULL){
				if(req==DISCONNECT){
				//close(*(friends[i].connfd));
				friends[i].connfd=NULL;
				friends[i].name = NULL;
				f_cnt--;	
				req = NULL;
				continue;
			}
			messages[res_size].talk = req;
			messages[res_size].sender = &friends[i];
			res_size++;
			if(res_size==length) return res_size;
			req = parse_request((friends[i].connfd));
		}
	}
	return res_size;
}
void non_block_mode(){
for(int i=0;i<f_cnt;i++){
		fcntl((friends[i].connfd),F_SETFL, O_NONBLOCK);
}
}

int clip(_message* messages, int length){
	int res_size = 0;
	for(int i=0;i<f_cnt;i++){
		//fcntl((friends[i].connfd),F_SETFL, O_NONBLOCK);
		char* req = parse_request((friends[i].connfd));
		while(req!=NULL){
			if(req==DISCONNECT){
				//close(*(friends[i].connfd));
				friends[i].connfd=NULL;
				friends[i].name = NULL;
				f_cnt--;	
				req = NULL;
				continue;
			}
			messages[res_size].talk = req;
			messages[res_size].sender = &friends[i];
			res_size++;
			if(res_size==length) return res_size;
			req = parse_request((friends[i].connfd));
			/*_parser parser;
			parser.index = 0;
			parser.prev_instruction = null;
			parse_instruction(req,&parser);
			_pl* pl;
			pl = pl_get(parser.instruction_array[0],1);
			//PLget(parser.instruction_array[0],1,pl);
			//TODO : message order
			if(pl->data=="contents"){
				pl = pl_get(parser.instruction_array[0],2);
				//PLget(parser.instruction_array[0],2,pl);
				messages[res_size].talk = pl->data;
				messages[res_size].friend = friends[i];
				if(res_size>=length) return res_size;
				res_size++;
			}
			req = parse_request(friends[i].connfd);*/
		}
	}
	return res_size;
}

void remove_friend(const char* name){
for(int i=0;i<f_cnt;i++){
		if(str_cmp(friends[i].name,name)){
				//printf("removing fd:%d\n",(friends[i].connfd));
				close((friends[i].connfd));
				friends[i].connfd=NULL;
				friends[i].name = NULL;
				f_cnt--;	
			return;
		}
	}
}

int request_friend(const char* name){
	int connfd;
	int result;
	struct sockaddr_un addr;
	connfd = socket(PF_LOCAL,SOCK_STREAM,0);
	memset(&addr,0,sizeof(addr));
	addr.sun_family=AF_LOCAL;
	char * ss = str_sum(prefix,name);
	strcpy(addr.sun_path,ss);
	if((connect(connfd,(struct sockaddr *)&addr,sizeof(addr)))==-1){
		printf("connect fail\n");
		free(ss);
		return -1;
	}
	//TODO : message formating
	char * reqstr = "(sof $s)\n(name $n)";
	_parser parser;
	parser.index = 0;
	parser.prev_instruction = null;
	parse_instruction(reqstr,&parser);
	_pl* pl = &(_pl){"primitive",my_name};
	pl_replace(parser.instruction_array[1],2,pl);
	//PLreplace(parser.instruction_array[1],2,pl)
	reqstr = "";
	reqstr = pl_str(parser.instruction_array[1]);
	//PLstr(parser.instruction_array[1],reqstr);
	char* its = i_to_s(str_len(reqstr));
	_pl* pl2 = &(_pl){"primitive",its};
	pl_replace(parser.instruction_array[0],2,pl2);
	//PLreplace(parser.instruction_array[0],2,pl2)
	char * header = "";
	header = pl_str(parser.instruction_array[0]);
	//PLstr(parser.instruction_array[0],header);
	char *ss2 = str_sum(header,"\n");
	char *ss3 = str_sum(ss2,reqstr);
	free(ss2);
	reqstr = ss3;
	write(connfd,reqstr,str_len(reqstr)+1);
	char* req = parse_request(connfd);
	_parser parser2;
	parser2.index = 0;
	parser2.prev_instruction = null;
	parse_instruction(req,&parser2);
	_pl* pl3;
	//pl3 = pl_get(parser2.instruction_array[0],1);
	PLget(parser2.instruction_array[0],1,pl3)
	if(str_cmp(pl3->data,"success")){
		//printf("comm fd:%d\n",connfd);
		friends[f_cnt].connfd = connfd;
		friends[f_cnt].name = name;
		f_cnt++;

		free(its);
		free(ss);
		free(ss3);
		return 1;
	}
	else{
		close(connfd);
		printf(pl3->data);
		printf("\n");
	       	printf("friend fail\n");
	}
	free(its);
	free(ss);
	free(ss3);
	return 0;
}

int init_file_comm(const char* name,int max_friend_cnt){
	char tmp[100];
	if(getcwd(tmp,sizeof(tmp))!=NULL){
		prefix = str_sum(tmp,"/comm/");
	}
	else {
		return 0;
	}
	friends = (_friend*)malloc(sizeof(_friend)*max_friend_cnt);
	//queue = (_message*)malloc(sizeof(_message)*max_queue);
	max_friends = max_friend_cnt;
	//max_queue = max_queue;
	my_name = name;
	return 1;
}
