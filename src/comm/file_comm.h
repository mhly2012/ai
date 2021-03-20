#pragma once
#include "picolisp.h"
#include "util/str.h"
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
typedef struct Friend{
	int  connfd;
	char* name;
} _friend;
typedef struct Message{
	_friend* sender;
	char* talk;
} _message;
int init_file_comm(const char*, int);
void* accept_friend(void*);
int request_friend(const char*);
void info(const char*, const char*);
int clip(_message*,int);

