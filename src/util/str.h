#pragma once
#include <stddef.h>
int str_cmp(const char* str1, const char* str2);
int str_len(const char* str);
void* mem_cpy(void* dst, const void* src, size_t n);
void* mem_move(void* dst, const void* src, size_t n);
char* str_sum(const char* str1,const char* str2);
int c_to_i(char c);
char i_to_c(int i);
char * i_to_s(int num);
int s_to_i(const char* s);
