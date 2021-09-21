#include "str.h"
int str_cmp(const char* str1, const char* str2){
	while(*str1!='\0'){
		if(*str2=='\0') return 0;
		if(*str1++!=*str2++) return 0;
	}
	if(*str2!='\0') return 0;
	return 1;
}
int str_contains(const char* target, const char* comp){
	int complen = str_len(comp);
	int tarlen = str_len(target);
	for(int t=0;t<tarlen;t++){
		if(target[t]==comp[0]){
			if(t+complen>tarlen) return -1;
			int i=1;
			for(;i<complen;i++){
				if(target[t+i]!=comp[i]) break;
			}
			if(i==complen) return t;
		}
	}
	return -1;
}
int sub_equal(const char* target,const char* comp, int index,int comp_len){
	for(int i=0;i<comp_len;i++){
		if(target[index+i]!=comp[i]) return 0;
	}
	return 1;
}
int str_cnt(const char* target, const char* comp){
	int cnt = 0;
	int comp_len = str_len(comp);
	for(int i=0;i<str_len(target)-comp_len;i++){
		if(sub_equal(target,comp,i,comp_len)){
			cnt++;
			i = i+comp_len;
		}
	}
	return cnt;
}
int str_len(const char* str){
	int i=0;
	while(str[i]!='\0') i++;
	return i;
}
void* mem_move(void* dst,const void* src, size_t n){
	char * d = dst;
	const char * s = src;
	if(dst>=src){
		while(n--) *d++=*s++;
	}else{
		d+=n;
		s+=n;
		while(n--) *--d=*--s;
	}
	return dst;
}
void* mem_cpy(void* dst, const void* src, size_t n){
	char *d = dst;
	const char * s = src;
	while(n--) *d++ = *s++;
	return dst;
}
void* mem_set(void* dst, int val, size_t len){
	unsigned char* ptr = dst;
	while(len-->0){
		*ptr++=val;
	}
	return dst;
}
char* str_sum(const char* str1,const char* str2){
	int size1 = str_len(str1);
	int size2 = str_len(str2);
	int size = size1 + size2;
	char* res = (char*)malloc(size+1);
	mem_cpy(res,str1,size1);
	mem_cpy(&res[size1],str2,size2+1);

	return res;
}
int c_to_i(char c){
	switch(c){
                case '0':
			return 0;
                case '1':
			return 1;
                case '2':
			return 2;
                case '3':
			return 3;
                case '4':
			return 4;
                case '5':
			return 5;
                case '6':
			return 6;
                case '7':
			return 7;
                case '8':
			return 8;
                case '9':
			return 9;
		default:
			return -1;
        }	
}
char i_to_c(int i){
	switch(i){
                case 0:
			return '0';
                case 1:
			return '1';
                case 2:
			return '2';
                case 3:
			return '3';
                case 4:
			return '4';
                case 5:
			return '5';
                case 6:
			return '6';
                case 7:
			return '7';
                case 8:
			return '8';
                case 9:
			return '9';
		default:
			return '\0';
        }	
}

char * i_to_s(int num){
	int quotient = num;
	int remainder = 0;
	int size = 1000;
	char* res = (char*)malloc(size+1);
	res[size]='\0';
	int i=0;
	while(quotient!=0){
		remainder = quotient%10;
		quotient = quotient/10;
		res[size-1-i] = i_to_c(remainder);
		i++;
		if(i==size) {
			size += 1000;
			char* tmp = (char*)malloc(size+1);
			mem_move(&tmp[1000],res,size+1);
			free(res);
			res = tmp;
		}
	}
	if(i!=(size-1)){
		char* tmp = (char*)malloc(i+1);
		mem_move(tmp,&res[size-i],i+1);
		free(res);
		res = tmp;
	}
	return res;
}

int s_to_i(const char* s){
	int res = 0;
	while(*s!='\0'){
		int i = c_to_i(*s);
		if(i<0) return -1;
		res = (res*10) + i;
		s++;
	}
	return res;	
}
