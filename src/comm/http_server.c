#define DEBUG_MODE
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include "util/debug.h"
#include "util/endian.h"
#include <stddef.h>
#include <errno.h>
#define LISTENQ 10

typedef struct phr_header {
	const char *name;
	size_t name_len;
	const char *value;
	size_t value_len;
}_header;
#define LINE_FEED_FLAG '\001'
#define CARRIAGE_RETURN_FLAG '\002'
#define NEWLINE_FLAG (LINE_FEED_FLAG|CARRIAGE_RETURN_FLAG)
#define COLON_FLAG '\004'
#define SPACE_FLAG '\008'
#define PROP_FLAG (COLON_FLAG|SPACE_FLAG)


void process_conn(int connfd, struct sockaddr_in *clientaddr){
	ssize_t rret;
	char buf_c;
	char buf[4096];
	unsigned short recent=0;
	unsigned char flag = 0;
	char* token;
	size_t token_len = 0;
	_header headers[100];
	unsigned short header_cnt = 0;
	headers[0].name = "METHOD";
	headers[0].name_len = 6;
	token = buf;
	while(1){
		while(((rret=read(connfd,&buf_c,sizeof(buf_c)))==-1)&&errno==EINTR); 
		if((buf_c>'\040')&&(buf_c<'\177')) {
			buf[recent++]=buf_c;
			token_len++;
//			debug_print(buf_c);
			if((flag&NEWLINE_FLAG)==NEWLINE_FLAG){
				headers[header_cnt].value = token;
				headers[header_cnt].value_len = token_len-1;
				header_cnt++;
				token = &buf[recent-1];	
				token_len = 1;
				printf("%.*s\n",headers[header_cnt-1].value_len,headers[header_cnt-1].value);
			}else if((flag&PROP_FLAG)==PROP_FLAG){
				headers[header_cnt].name = token;
				headers[header_cnt].name_len = token_len-2;
				token = &buf[recent-1];
				token_len = 1;
				printf("%.*s\n",headers[header_cnt].name_len,headers[header_cnt].name);
				fflush(stdout);
			}
			flag&=~(NEWLINE_FLAG|PROP_FLAG);
			if(buf_c==':') flag|=COLON_FLAG;
		}
		else if(buf_c=='\012'){
			if(flag&LINE_FEED_FLAG) {
				debug_print("end");
				break;
			}else {
				flag |= LINE_FEED_FLAG;
			}
		}else if(buf_c=='\015'){
			flag|=CARRIAGE_RETURN_FLAG;
		}else if(buf_c==' '){
			flag|=SPACE_FLAG;
		}
		else{	
			flag&=~(NEWLINE_FLAG|PROP_FLAG);
		}
		if(rret<=0) {
			debug_print("fail");
			break;
		}
	}
	fflush(stdout);
	/*
	char buf[4096], *method, *path;
	struct phr_header headers[100];
	size_t bufcur=0,prevcur=0,method_len,path_len,num_headers;
	ssize_t rret2;
	while(1){
		while((rret = read(connfd,buf+bufcur,sizeof(buf)-bufcur)==-1)&&errno==EINTR);
		if(rret<=0) {
			debug_print("ioerror");
			break;
		}
		prevcur = bufcur;
		bufcur += rret;
		const char *buf=buf_start,*buf_end = buf_start+len;
		size_t max_headers = *num_headers;
		int r;
		if(prevcur!=0&&is_complete(buf,buf_end,last_len,&r)==NULL){
			
		}
	}
	*/
}
int inet_server(){
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	socklen_t clientlen = sizeof(clientaddr);
	int connfd;
	int listenfd, optval = 1;
	listenfd = socket(PF_INET, SOCK_STREAM,0);
	if(setsockopt(listenfd, SOL_SOCKET,SO_REUSEADDR,(const void*)&optval,sizeof(int))<0) return 0;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	unsigned char addr[4] = { 127, 0, 0, 1 };
	u_short port = 0;
	change_endian((u_short)43295,port);
	serveraddr.sin_addr.s_addr = *((unsigned int*)addr);
	serveraddr.sin_port = port;
	if(bind(listenfd, (struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
		return 0;
	}
	if(listen(listenfd,LISTENQ)<0) return 0;
	signal(SIGPIPE, SIG_IGN);
	while(1){
		connfd = accept(listenfd, &clientaddr,&clientlen);
		int cid = fork();
		if(cid==0){
			process_conn(connfd,&clientaddr);
			exit(1);
		}
		else if(cid>0){
			debug_print("fork success\n");
		}
		else debug_print("fork error\n");
		debug_print(cid);
		close(connfd);
	}
}

int main(int argc, char **argv){
	if(argc<2){
		debug_print("input file missed.\n");
		return 0;
	}
	inet_server();
	return 0;
}
