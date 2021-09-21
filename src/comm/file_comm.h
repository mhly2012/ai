#pragma once
#include "picolisp.h"
#include "util/str.h"
#include <sys/socket.h>   // unix local(file) socket사용하기 위한 헤더
#include <netinet/tcp.h>  // 
#include <sys/un.h>. //
#include <sys/unistd.h>. //
#include <sys/types.h>.  //
#include <arpa/inet.h> //
#include <pthread.h> // pthread cond wait signal 헤더
#include <signal.h>  // kill 과 handler  헤더
//#include <sys/event.h> // kqueue 사용하기 위한 헤더
//#include <sys/select.h>
//#include <stdio.h>
//#include <signal.h>
//#include <stddef.h>
#include <errno.h>   // read 함수 에러처리하기 위한 헤더
#include <fcntl.h>   // 논블록 모드 사용위한 헤더

typedef struct Friend{
	int  connfd;
	char* name;
	pid_t pid;
} _friend;
typedef struct Message{
	_friend* sender;
	char* talk;
} _message;
int init_file_comm(const char*, int);
void* accept_friend(void*);
//void* accept_friend_non_block(void*);

int request_friend(const char*);
void info(const char*, const char*);
int clip(_message*,int);
void remove_friend(const char* name);
void clip_blocking(const char* name,_message* messages);

static pthread_mutex_t message_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t message_cond = PTHREAD_COND_INITIALIZER;
void kill_handler(int signum);
