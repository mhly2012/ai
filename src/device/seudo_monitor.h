#pragma once
#include <termios.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
#include <pthread.h>
#include <signal.h>  // to handle ctrl-c interrupt
#include <fcntl.h>  // to call fcntl 

#include "picolisp.h"
#include "util/debug.h"
#include "comm/file_comm.h"
