#include "picolisp.h"

_pl* parse_set(char *string,_parser *parser);
bool is_blank(char c){
	switch(c){
		case ' ':
		case '\n':
		case '\t':
			return true;
		default:
			return false;
	}
}
char* next_token2(const char* string){
	if(!is_blank(*string)){
		char *tmp = (char*)malloc(sizeof(char)*20);
		if(*string=='('){
			tmp[0]='(';
			tmp[1]='\0';
			debug_print(tmp);
			string++;
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			return tmp;
		}
		else if(*string==')'){
			tmp[0]=')';
			tmp[1]='\0';
			debug_print(tmp);
			string++;
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			return tmp;
		}
		else if(*string==null){
			debug_print("string end");
			free(tmp);
			return null;
		}
		tmp[0] = *string;
		string++;
		int tmp_size = 1;
		while(!is_blank(*string)){
			if(*string==')') break;
			tmp[tmp_size] = *string;
			tmp_size++;
			string++;		
		}
		tmp[tmp_size] = '\0';
		debug_print(tmp);
		tmp = (char*)realloc(tmp,sizeof(char)*(tmp_size+1));
		return tmp;		
	}
	else{
		string++;
		return next_token2(string);	
	}
}
char* next_token(char *string, _parser *parser){
	if(!is_blank(string[parser->index])){
		char *tmp = (char*)malloc(sizeof(char)*20);
		if(string[parser->index]=='('){
			tmp[0]='(';
			tmp[1]='\0';
			debug_print(tmp);
			parser->index++;
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			return tmp;
		}
		else if(string[parser->index]==')'){
			tmp[0]=')';
			tmp[1]='\0';
			debug_print(tmp);
			parser->index++;
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			return tmp;
		}
		else if(string[parser->index]==null){
			debug_print("string end");
			free(tmp);
			return null;
		}
		tmp[0] = string[parser->index];
		parser->index++;
		int tmp_size = 1;
		while(!is_blank(string[parser->index])){
			if(string[parser->index]==')') break;
			tmp[tmp_size] = string[parser->index];
			tmp_size++;
			parser->index++;		
		}
		tmp[tmp_size] = '\0';
		debug_print(tmp);
		tmp = (char*)realloc(tmp,sizeof(char)*(tmp_size+1));
		return tmp;		
	}
	else{
		parser->index++;
		return next_token(string,parser);	
	}	
}
char* open_file(char *file_name){
	FILE *file = fopen(file_name,"rb");
	fseek(file,0,SEEK_END);
	long file_size = ftell(file);
	fseek(file,0,SEEK_SET);

	char *string = malloc(file_size+1);
	fread(string,1,file_size,file);
	fclose(file);
	string[file_size]=null;
	return string;
}


_pl* parse_primitive(char* token){
	_pl* pl = (_pl*)malloc(sizeof(_pl));
	switch(token[0]){
		case '\'':
			break;
		case '"':
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
	}	
}
_pl* parse_element(char *string, _parser *parser, char* tok){
	if(tok[0]=='('){
		return parse_set(string,parser);	
	}
	else{
		_pl* element = (_pl*)malloc(sizeof(_pl));
		element->type = "primitive";
		element->data = tok;
		return element;
	}
		 
}
_pl* parse_set(char *string,_parser *parser){
	_pl* set = (_pl*)malloc(sizeof(_pl));
	set->type="set";
	_chain* chain = (_chain*)malloc(sizeof(_chain));
	_chain* first = chain;
	int cnt=0;
	cnt = 0;
	char *tok = next_token(string, parser);
	while(tok[0]!=')'){
		_pl* element = parse_element(string,parser,tok);
		chain->element = element;
		chain->next = (_chain*)malloc(sizeof(_chain));
		chain = chain->next;
		cnt++;
		tok = next_token(string,parser);
	}
	chain->next = null;
	set->array = (_pl**)malloc(sizeof(_pl*)*cnt);
	set->size =cnt;
	int i;
	chain = first;
	for(i=0;i<set->size;i++){
		set->array[i]=chain->element;
		chain = chain->next;
	}
	return set;
}
//eval
//TODO : char* -> const char*
void parse_instruction(char *string,_parser *parser){
	char *tok = next_token(string, parser);
	while(tok!=null){

		_psi *instruction = (_psi*)malloc(sizeof(_psi));
		instruction->prev = parser->prev_instruction;
		parser->prev_instruction = instruction;
                if(tok[0]=='('){
                        instruction->function = parse_set(string,parser);
                }
                else{
                        _pl *eval = (_pl*)malloc(sizeof(_pl));
                        eval->type="primitive";
                        eval->data = tok;
                        instruction->function = eval;
                }
		
		tok = next_token(string,parser);
		
        }
	int cnt=0;
	_psi* inst = parser->prev_instruction;
	while(inst!=null){
		debug_print(inst->function->type);
		inst = inst->prev;
		cnt++;
		debug_print("iter");
	}
	parser->instruction_array = (_pl**)malloc(sizeof(_pl*)*cnt);
	parser->array_size = cnt;
	inst = parser->prev_instruction;
	for(;cnt>0;cnt--){
		parser->instruction_array[cnt-1] = inst->function;
		inst = inst->prev;
	}
}

int first_order_int_array(const char* string,int* buf,int size){
	string++;
	char *tok = next_token2(string);
	for(int i=0;i<size;i++){
		if(tok[0]=='('){
			tok = next_token2(string);
			i--;
                }
		else if(tok[0]==')'){
			return i;
		}
		else{
			//buf[i++] = s_to_i(tok);
			tok = next_token2(string);
		}
	}
	return size;
}
/*
int main(int argc, char **argv){
	if(argc<2){
	printf("input file missed.\n");
	return 0;
	}
	char *contents = open_file(argv[1]);
	
	_parser parser;
	parser.index = 0;
	parser.prev_instruction = null;
	parse_instruction(contents,&parser);
	
	printf("hello\n");
	return 0;
}*/
