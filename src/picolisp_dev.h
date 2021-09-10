#pragma once
#include <unistd.h> //to declare STDOUT_FILENO, to call sleep, read, getcwd
//#include <stdio.h>
#include <stdlib.h> //to call malloc, free
#include <string.h> //to call strcpy, memset

#include "util/debug.h"
#include "util/str.h"

#define true 1
#define false 0
#define bool int
#define null ((void*)0)
#define MAXIMUM_KEYWORD_LENGTH 10
#define DEBUG_MODE

typedef struct PicoList _pl;
typedef struct PicoSymbolicInstruction _psi;

// 1. PL 조작함수에서 depth 구조 처리하기 위한 구조체
typedef struct PicoJob{
	_pl* pl;
	int index;
} _pj;
typedef struct PicoStack _ps;
typedef struct PicoStack {
	_pj* job;
	_ps* base;
} _ps;
// 1. depth 구조 처리 구조체 끝
				

/*
typedef union PrimitiveData{
        int _int;
        char* string;
        long _long;
        int* _int_array;
        long* _long_array;
        float _float;
        float* _float_array;
} _prim;
*/

// 2. PL parsing할 때 인스트럭션 정리하기 위한 구조체
typedef struct Chain _chain;
typedef struct Chain{
	_pl* element;
	_chain* next;
} _chain;
// 2. 인스트럭션 정리 구조체 끝

typedef struct PicoList{
        char* type;  // name: primitive, set
        union{
		struct{
			_pl** array;
			int size;
		};
                char* data; // primitive data
        };
} _pl;
typedef struct PicoSymbolicInstruction{
        _psi* prev;
        _pl* function;
} _psi;

typedef struct Parser{
        int index;
        _psi* prev_instruction;
        _pl** instruction_array;
	int array_size;
} _parser;
// TODO : char* -> const char*
void parse_instruction(char *string,_parser *parser);
//char* open_file(char *file_name);
//int first_order_int_array(const char *,int*buf,int size);

// 3. PL 조작함수들
_pl* pl_get(_pl* root,int index);
void pl_replace(_pl* root, int index, _pl* rep);
char* pl_str(_pl* root);
// 3. PL 조작함수들 끝
