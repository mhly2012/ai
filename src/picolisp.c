#include "picolisp.h"

_pl* parse_list(char *string,_parser *parser);
_pl* pl_get(_pl* root,int index){
// index x -> 0 is fault, 1,2,3,..., 몫은 사이즈와 그 부분집합 내에서 인덱스를 >담고, 나머지가 루트에서 부분집합 자체의 인덱스를 가리킨다.	
	int quot = index;
	int rem;
	_pl* ret = root;
	do{
		if(str_cmp(ret->type,"set")==0){
			ret = null;
			quot = 0;
		}
		else{
			rem = quot%(ret->size+1);
			if(rem==0){
				ret = null;
				quot = 0;
			}
			else{
				quot = quot / (ret->size+1);
				ret = ret->array[rem-1];
			}
		}
	}while(quot!=0);
	return ret;
}

void pl_replace(_pl* root, int index, _pl* rep){
	int quot = index;
	int rem;
	_pl* tmp = root;
	do{
		if(str_cmp(tmp->type,"set")==0){
			tmp = null;
			quot=0;
		}else{
			rem = quot % (tmp->size+1);
			if(rem ==0){
				tmp = null;
				quot=0;
			}else{
				quot = quot / (tmp->size+1);
				if(quot==0){
					tmp->array[rem-1] = rep;
				}
				else{
					tmp = tmp->array[rem-1];
				}
			}
		}
	}while(quot!=0);
}


char* pl_str(_pl* root){
	//_pl* tmp = root;
	_pj* pj = &(_pj){root,0};
	_ps* ps = &(_pl){pj,NULL};
	char* ret = (char*)malloc(1);
	ret[0] = '\0';
	while(ps!=NULL){
		_pj* curjob = ps->job;
		int jidx = curjob->index;
		if(jidx ==0){
			char* tmp = ret;
			ret = str_sum(ret,"(");
			free(tmp);
		}
		if(str_cmp(curjob->pl->array[jidx]->type,"set")){
			_pj* tmpjob = {curjob->pl->array[jidx],0};
			_ps* tmpps = {tmpjob,ps};
			ps = tmpps;
		}else if(str_cmp(curjob->pl->array[jidx]->type,"primitive")){
			if(jidx!=0){
				char* tmp = ret;
				ret = str_sum(ret," ");
				free(tmp);
			}
			char* tmp = ret;
			ret = str_sum(ret,curjob->pl->array[jidx]->data);
			free(tmp);
			curjob->index++;
			jidx++;
		}
		if(jidx==curjob->pl->size){
			char* tmp = ret;
			ret = str_sum(ret,")");
			free(tmp);
			ps = ps->base;
		}
	}
	return ret;
}

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
		else if(string[parser->index]=='['){
			tmp[0]='[';
			tmp[1]='\0';
			debug_print(tmp);
			parser->index++;
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			return tmp;
		}else if(string[parser->index]==']'){
			tmp[0]=']';
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

/*
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
*/
_pl* parse_element(char *string, _parser *parser, char* tok){
	if(tok[0]=='('){
		return parse_set(string,parser);	
	}
	if(tok[0]=='['){
		return parse_list(string,parser);
	}
	else{
		_pl* element = (_pl*)malloc(sizeof(_pl));
		element->type = "primitive";
		element->data = tok;
		return element;
	}
}
// combination
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
// permutation
_pl* parse_list(char *string,_parser *parser){
	_pl* list = (_pl*)malloc(sizeof(_pl));
	list->type="list";
	_chain* chain = (_chain*)malloc(sizeof(_chain));
	_chain* first = chain;
	int cnt=0;
	cnt = 0;
	char *tok = next_token(string, parser);
	while(tok[0]!=']'){
		_pl* element = parse_element(string,parser,tok);
		chain->element = element;
		chain->next = (_chain*)malloc(sizeof(_chain));
		chain = chain->next;
		cnt++;
		tok = next_token(string,parser);
	}
	chain->next = null;
	list->array = (_pl**)malloc(sizeof(_pl*)*cnt);
	list->size =cnt;
	int i;
	chain = first;
	for(i=0;i<list->size;i++){
		list->array[i]=chain->element;
		chain = chain->next;
	}
	return list;
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
		else if(tok[0]=='['){
			instruction->function = parse_list(string,parser);
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
