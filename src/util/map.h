#pragma once
#include "str.h"
#define SWITCH_STRING(key) \
{\
        static _map* str_table;\
        static bool table_init = false;\
	if(!table_init) str_table = _map_static.instantiate();\
        int nKey = -1;\
SWITCH_RESTART:\
        if(table_init) nKey = str_table->get(str_table,key); \
        switch(nKey){\
                case -1:{
#define CASE(token)\
                }case __LINE__: \
                        if(!table_init) str_table->put(str_table,token,__LINE__);\
                        else {
#define DEFAULT()\
                }default:\
                        if(table_init){
#define END_SWITCH()    }if(!table_init) {table_init = true;goto SWITCH_RESTART; }}}

typedef struct CharInt _char_int;
typedef struct Map _map;
void put_impl(_map* self, char* key, int value);
int get_impl(_map* self, char* key);
_map* instantiate_impl();
void delete_impl(_map* target);
typedef struct CharInt{
	char* key;
	int value;
	_char_int* next;
} _char_int;
typedef struct Map{
	_char_int* ci;
	void (*put)(_map* self,char* key,int value);
	int (*get)(_map* self, char* key);
	_map* (*instantiate)();
	void (*delete)(_map* target);
} _map;
static const _map _map_static = {null,put_impl,get_impl,instantiate_impl,delete_impl};
void put_impl(_map* self, char* key, int value){
	if(self->ci==null) {
		self->ci = (_char_int*)malloc(sizeof(_char_int));
		self->ci->key = key;
		self->ci->value = value;
		self->ci->next = null;
		return;
	}	
	_char_int* ci = self->ci;
	while(ci->next!=null){
		if(str_cmp(ci->key,key)) {
			ci->value = value;
			return;
		}
		ci=ci->next;
	}
	ci->next = (_char_int*)malloc(sizeof(_char_int));
	ci->next->key = key;
	ci->next->value = value;
	ci->next->next = null;
	return;
}
int get_impl(_map* self, char* key){
	if(self->ci==null) return 0;
	_char_int* ci = self->ci;
	while(ci->next!=null){
		if(str_cmp(ci->key,key)) return ci->value;
		ci=ci->next;
	}
	return 0;
}
_map* instantiate_impl(){
	_map* res = (_map*)malloc(sizeof(_map));
	memcpy(res,&_map_static,sizeof(_map));
	return res;
}
void delete_impl(_map* target){
	
}
