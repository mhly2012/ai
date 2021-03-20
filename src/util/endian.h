#pragma once
#define change_endian(x,r) for(int cei=0;cei<sizeof(x)/2;cei++) \
                                r|=(x&(0xff<<((sizeof(x)-cei-1)*8)))>>((sizeof(x)-(2*cei)-1)*8);\
			for(int cei=0;cei<sizeof(x)/2;cei++)\
				r|=(x&(0xff<<(cei*8)))<<((sizeof(x)-(2*cei)-1)*8);\
			if(sizeof(x)%2)\
			r|=(x&(0xff<<(sizeof(x)/2)));
