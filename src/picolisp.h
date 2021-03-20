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
// index x -> 0 is fault, 1,2,3,..., 몫은 사이즈와 그 부분집합 내에서 인덱스를 담고, 나머지가 타겟 부분집합 자체의 인덱스를 가리킨다.
/*
#define PLget(target, element_number ,response) \
			{int quotient = element_number;\
			int remainder;\
			response = target;\
			do{\
			if(str_cmp(response->type,"set")==0) { \
				response = null;\
				quotient=0;}\
			else{\
			remainder = quotient % (response->size+1);\
			if(remainder==0){\
				response = null; quotient=0;\
			}else{\
				quotient = quotient / (response->size+1);\
				response = response->array[remainder-1];\
			}\
			}\
			}while(quotient!=0);}
*/
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
/*
#define PLreplace(target, element_number, newer) \
			{int pl_rep_quotient= element_number;\
			int pl_rep_remainder;\
			_pl* pl_rep_tmp = target;\
			if(_Generic(newer,_pl*:1,default:0)) {\
			do{\
			if(str_cmp(pl_rep_tmp->type,"set")==0) { pl_rep_tmp = null;\
				pl_rep_quotient=0;}\
			else{\
			pl_rep_remainder = pl_rep_quotient % (pl_rep_tmp->size+1);\
			if(pl_rep_remainder==0){\
				pl_rep_tmp = null; pl_rep_quotient=0;\
			}else{\
				pl_rep_quotient = pl_rep_quotient / (pl_rep_tmp->size+1);\
				if(pl_rep_quotient==0){pl_rep_tmp->array[pl_rep_remainder-1]=newer;}\
				else{pl_rep_tmp = pl_rep_tmp->array[pl_rep_remainder-1];}\
			}\
			}\
			}while(pl_rep_quotient!=0);\
			}}
//#define PLiter(pl, element, statement) \


#define PLstr(target, str_buf) \
			{int pl_str_loop = 1;\
			_pl* pl_str_tmp = target;\
			_pj* pj = &(_pj){target,0};\
			_ps* ps = &(_ps){pj,NULL};\
			str_buf = (char*)malloc(1);\
			str_buf[0] = '\0';\
			while(ps!=NULL){\
				_pj* curjob = ps->job;\
				int jindex = curjob->index;\
				if(jindex == 0){\
					char * tmp = str_buf;\
					str_buf = str_sum(str_buf,"(");\
					free(tmp);\
				}\
				if(str_cmp(curjob->pl->array[jindex]->type,"set")){\
					_pj* tmpjob = {curjob->pl->array[jindex],0};\
					_ps* tmpps = {tmpjob,ps};\
					ps = tmpps;\
				}\
				else if(str_cmp(curjob->pl->array[jindex]->type,"primitive")){\
					if(jindex!=0) {\
						char * tmp = str_buf;\
						str_buf = str_sum(str_buf," ");\
						free(tmp);\
					}\
					char *tmp = str_buf;\
					str_buf = str_sum(str_buf,curjob->pl->array[jindex]->data);\
					free(tmp);\
					curjob->index++;\
					jindex++;\
				}\
				if(jindex == curjob->pl->size){\
					char *tmp = str_buf;\
					str_buf = str_sum(str_buf,")");\
					free(tmp);\
					ps = ps->base;\
				}\
			}}
			
*/				
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
int first_order_int_array(const char *,int*buf,int size);
_pl* pl_get(_pl* root,int index);
void pl_replace(_pl* root, int index, _pl* rep);
char* pl_str(_pl* root);
