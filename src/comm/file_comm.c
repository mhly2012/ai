#define  DEBUG_MODE
#include "file_comm.h"
#define LISTENQ 10
#define DISCONNECT "(disconnect)"
static _friend *friends=NULL;
static int max_friends;
static char *my_name;
static int f_cnt;
static char* prefix;

char* parse_request(int connfd){
	ssize_t rret;
	char buf_c;
	int buf_max = 100;
	char* buf = (char*)malloc(buf_max);
	size_t bufcur=0,prevcur=0;
	size_t size;
	while(1){
		while(((rret=read(connfd,&buf_c,sizeof(buf_c)))==-1)&&errno==EINTR);

		if((rret==0)||(errno==ECONNRESET)) {
			return DISCONNECT;
		}
		if(errno==EAGAIN) {
			//err(EXIT_FAILURE,"reading sof");
			//printf("reading sof\n");
			return NULL;
		}
		buf[bufcur] = buf_c;
		bufcur++;
		if(bufcur==buf_max) {
			buf_max*=2;
			buf = (char*)realloc(buf,buf_max);
		}
		if(buf_c=='\n') {
			buf[bufcur] = null;
			_parser parser;
			parser.index = 0;
			parser.prev_instruction = null;
			parse_instruction(buf,&parser);
			_pl* pl = pl_get(parser.instruction_array[0],1);
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
			//err(EXIT_FAILURE,"parse main pl");
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

		// 서버의 pid를 파일에 등록하여 클라이언트에서 조회한 후 kill을 보낼 수 있게 함
		pid_t pid = getpid(); 
		
		char* req = parse_request(connfd);

		if(req!=NULL){
			_parser parser;
			parser.index = 0;
			parser.prev_instruction = null;
			parse_instruction(req,&parser);
			_pl* pl;
			pl = pl_get(parser.instruction_array[0],1);

			if(str_cmp(pl->data,"name")){
				pl = pl_get(parser.instruction_array[0],2);
				char * name = pl->data;
				//TODO : add friend

				friends[f_cnt].connfd = connfd;
				friends[f_cnt].name = name;
				friends[f_cnt].pid = s_to_i(pl_get(parser.instruction_array[1],2)->data);
				f_cnt++;

				char * suc_res_str = "(sof $s)\n(success)\n(pid $c)";
				_parser srs_parser;
				srs_parser.index = 0;
				srs_parser.prev_instruction = null;
				parse_instruction(suc_res_str,&srs_parser);
				pid_t pid = getpid();
				_pl* pid_pl = &(_pl){"primitive",i_to_s(pid)};
				pl_replace(srs_parser.instruction_array[2],2,pid_pl);
				char* str1 = pl_str(srs_parser.instruction_array[1]);
				char* str2 = pl_str(srs_parser.instruction_array[2]);
				suc_res_str = str_sum(str1,str2);
				//free(str1);	
				//free(str2);
				char* its = i_to_s(str_len(suc_res_str));
				_pl* pl2 = &(_pl){"primitive",its};
				pl_replace(srs_parser.instruction_array[0],2,pl2);
				char * header = "";
				header = pl_str(srs_parser.instruction_array[0]);
				char *ss2 = str_sum(header,"\n");
				char *ss3 = str_sum(ss2,suc_res_str);
				//free(suc_res_str);
				free(ss2);
				suc_res_str = ss3;
				write(connfd,suc_res_str,str_len(suc_res_str)+1);
				//write(connfd,"(sof 9)\n(success)\0",18);
				int ori = fcntl(connfd,F_GETFL);
				//fcntl((connfd),F_SETFL, ori|O_NONBLOCK);
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
			write((friends[i].connfd),message,str_len(message)+1);
			kill(friends[i].pid,SIGURG);

			return;
		}
	}
}


void clip_blocking(const char* name,_message* messages){
	int res_size = 0;
	for(int i=0;i<f_cnt;i++){
		if(!str_cmp(friends[i].name,name)) continue;
		//printf("block fd:%d\n",(friends[i].connfd));
		int ori = fcntl((friends[i].connfd),F_GETFL);
		int fre =  ori&~(O_NONBLOCK);
		fcntl((friends[i].connfd),F_SETFL, fre);
		char* req = parse_request((friends[i].connfd));
		
		if(req==DISCONNECT){
			//close(*(friends[i].connfd));
			friends[i].connfd=NULL;
			friends[i].name = NULL;
			if(i+1<f_cnt){
				mem_move(&friends[i],&friends[i+1],f_cnt-i);
			}
			i--;
			f_cnt--;	
			req = NULL;
			continue;
		}
		messages[res_size].talk = req;
		messages[res_size].sender = &friends[i];
		fcntl((friends[i].connfd),F_SETFL, ori);
	}
}

int clip(_message* messages, int length){
	int res_size = 0;
	for(int i=0;i<f_cnt;i++){
		char* req = parse_request(friends[i].connfd);
		if(req!=NULL){
			if(req==DISCONNECT){
				friends[i].connfd=NULL;
				friends[i].name = NULL;
				if(i+1<f_cnt){
					mem_move(&friends[i],&friends[i+1],f_cnt-i);
				}
				i--;
				f_cnt--;	
			}else{
				messages[res_size].talk = req;
				messages[res_size].sender = &friends[i];
				res_size++;
			}
		}
		else{
			
		//	err(EXIT_FAILURE,"clip");
		}
	}
	return res_size;
}

void remove_friend(const char* name){
int index = -1;
for(int i=0;i<f_cnt;i++){
		if(str_cmp(friends[i].name,name)){
				index = i;
				close((friends[i].connfd));
				friends[i].connfd=NULL;
				friends[i].name = NULL;
				if(i+1<f_cnt){
					mem_move(&friends[i],&friends[i+1],f_cnt-i);
				}
				i--;
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
	char * reqstr = "(sof $s)\n(name $n)\n(pid $c)";
	_parser parser;
	parser.index = 0;
	parser.prev_instruction = null;
	parse_instruction(reqstr,&parser);
	_pl* pl = &(_pl){"primitive",my_name};
	pl_replace(parser.instruction_array[1],2,pl);
	pid_t pid = getpid();
	_pl* pid_pl = &(_pl){"primitive",i_to_s(pid)};
	pl_replace(parser.instruction_array[2],2,pid_pl);
	char* str1 = pl_str(parser.instruction_array[1]);
	char* str2 = pl_str(parser.instruction_array[2]);
	reqstr = str_sum(str1,str2);
	//free(str1);	
	//free(str2);
	char* its = i_to_s(str_len(reqstr));
	_pl* pl2 = &(_pl){"primitive",its};
	pl_replace(parser.instruction_array[0],2,pl2);
	char * header = "";
	header = pl_str(parser.instruction_array[0]);
	char *ss2 = str_sum(header,"\n");
	char *ss3 = str_sum(ss2,reqstr);
	//free(reqstr);
	free(ss2);
	reqstr = ss3;

	write(connfd,reqstr,str_len(reqstr)+1);
	// 친구요청을 보내고 응답메시지를 기다림
	// 한번만 5초간 메시지를 기다림
	char* req = parse_request(connfd);
	_parser parser2;
	parser2.index = 0;
	parser2.prev_instruction = null;
	parse_instruction(req,&parser2);
	_pl* pl3;
	pl3 = pl_get(parser2.instruction_array[0],1);
	if(str_cmp(pl3->data,"success")){
		// 연결 성공
		_pl* pid_pl = pl_get(parser2.instruction_array[1],1);
		friends[f_cnt].pid = s_to_i(pid_pl->data);
		friends[f_cnt].connfd = connfd;
		friends[f_cnt].name = name;
		int ori = fcntl(connfd,F_GETFL);
		//fcntl(connfd,F_SETFL,ori|O_NONBLOCK);
		f_cnt++;
		free(its);
		free(ss);
		free(ss3);
		return 1;
	}
	else{
		// 실패
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
	max_friends = max_friends+max_friend_cnt;
	if(friends!=NULL){
		friends = (_friend*)realloc(friends,sizeof(_friend)*max_friends);
		
	}else{
		friends = (_friend*)malloc(sizeof(_friend)*max_friends);
	}
	my_name = name;
	return 1;
}
void kill_handler(int signum){
//	printf("%d\n",signum);
//	pthread_mutex_lock(&message_lock);
	
	pthread_cond_signal(&message_cond);

//	pthread_cond_wait(&message_cond,&message_lock);
//	pthread_mutex_unlock(&message_lock);
}
