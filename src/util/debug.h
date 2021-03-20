#pragma once
#include <stdio.h>
#ifdef DEBUG_MODE
#define debug_print(x) switch(_Generic(x,char*:0,char:1,int:2,float:3,unsigned long:4,unsigned short:5,default:10))\
			{case 0:\
			printf("%s",x);break;\
			case 1:\
			printf("%c",x);break;\
			case 2:\
			printf("%d",x);break;\
			case 3:\
			printf("%f",x);break;\
			case 4:\
			printf("%lu",x);break;\
			case 5:\
			printf("%hu",x);break;\
			default:\
			printf("%s",x);}
#define debug_print_hex(x) switch(_Generic(x,char*:0,char:1,int:2,float:3,unsigned long:4,unsigned short:5,default:10))\
                        {case 0:\
                        printf("%x",x);break;\
                        case 1:\
                        printf("%x",(unsigned char)x);break;\
                        case 2:\
                        printf("%x",x);break;\
                        case 3:\
                        printf("%x",x);break;\
                        case 4:\
                        printf("%lx",x);break;\
                        case 5:\
                        printf("%hx",x);break;\
                        default:\
                        printf("%x",x);}
#else
#define debug_print(x)
#define debug_print_hex(x)
#endif

