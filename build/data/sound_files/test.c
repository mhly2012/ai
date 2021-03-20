#include <stdio.h>

int main(){
	FILE* pipe = popen("cat 25.wav","r");

	if(!pipe){
		printf("error\n");
	}
	else{
		char buffer[100];
		fread(buffer,1,100,pipe);
		printf(buffer);
		printf("\n");
	}
	return;
}
