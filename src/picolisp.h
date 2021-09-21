#pragma once
#include <unistd.h> //to declare STDOUT_FILENO, to call sleep, read, getcwd
//#include <stdio.h>
#include <stdlib.h> //to call malloc, free
#include <string.h> //to call strcpy, memset
#include <err.h>  //to call err

#include "util/debug.h"
#include "util/str.h"

#define true 1
#define false 0
#define bool int
#define null ((void*)0)
#define MAXIMUM_KEYWORD_LENGTH 10
#define DEBUG_MODE
// index x -> 0 is fault, 1,2,3,..., 몫은 사이즈와 그 부분집합 내에서 인덱스를 담고, 나머지가 타겟 부분집합 자체의 인덱스를 가리킨다.

typedef struct PicoList _pl;
typedef struct PicoJob{
	_pl* pl;
	int index;
} _pj;
typedef struct PicoStack _ps;
typedef struct PicoStack {
	_pj* job;
	_ps* base;
} _ps;
typedef struct PicoList _pl;
typedef struct PicoSymbolicInstruction _psi;
typedef struct Chain _chain;
typedef union PrimitiveData{
        int _int;
        char* string;
        long _long;
        int* _int_array;
        long* _long_array;
        float _float;
        float* _float_array;
} _prim;
typedef struct Chain{
	_pl* element;
	_chain* next;
} _chain;
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
char* open_file(char *file_name);
_pl* pl_get(_pl* root,int index);
void pl_replace(_pl* root, int index, _pl* rep);
char* pl_str(_pl* root);
