#include "picolisp.h"

_pl* parse_list(char *string,_parser *parser);
_pl* pl_get(_pl* root,int index){
// index x -> 0 is fault, 1,2,3,..., 몫은 사이즈와 그 부분집합 내에서 인덱스를 >담고, 나머지가 루트에서 부분집합 자체의 인덱스를 가리킨다.	
	int quot = index;
	int rem;
	_pl* ret = root;
	do{
		if(str_cmp(ret->type,"set")||str_cmp(ret->type,"list")){
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
		if(str_cmp(tmp->type,"set")||str_cmp(tmp->type,"list")){
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
		if(str_cmp(curjob->pl->array[jidx]->type,"set")||str_cmp(curjob->pl->array[jidx]->type,"set")){
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

void pl_addElement(_pl* root,_pl* ele){
	if(root->type=="primitive"){
		debug_print("pl_addelement_error");
		//error
		return;
	}
	_pl* narr[root->size+1];
	mem_cpy(narr,root->array,root->size*sizeof(_pl*));
	narr[root->size] = ele;
	free(root->array);
	root->array = narr;
	root->size++;
}


_pl* parse_set(char *string,_parser *parser);
bool is_blank(char c){
	switch(c){
		case '\r':
		case ' ':
		case '\n':
		case '\t':
			return true;
		default:
			return false;
	}
}

int get_token(const char *string, char *res);
int get_token(const char *string, char *res){
	int index=0;
	if(!is_blank(string[index])){
		char *tmp = (char*)malloc(sizeof(char)*20);
		if(string[index++]=='('){
			tmp[0]='(';
			tmp[1]='\0';
			debug_print(tmp);
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			res = tmp;
			return index;
		}
		else if(string[index++]==')'){
			tmp[0]=')';
			tmp[1]='\0';
			debug_print(tmp);
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			res = tmp;
                        return index;	
		}
		else if(string[index++]=='['){
			tmp[0]='[';
			tmp[1]='\0';
			debug_print(tmp);
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			res = tmp;
                        return index;	
		}else if(string[index++]==']'){
			tmp[0]=']';
			tmp[1]='\0';
			debug_print(tmp);
			tmp = (char*)realloc(tmp,sizeof(char)*2);
			res = tmp;
                        return index;
		}
		else if(string[index]=='\0'){
			debug_print("string end");
			free(tmp);
			res = null;
                        return 1;
		}
		tmp[0] = string[index++];
		int tmp_size = 1;
		while(!is_blank(string[index])){
			if((string[index]==')')||(string[index]==']')||(string[index]=='\0')) break;
			tmp[tmp_size++] = string[index++];
		}
		tmp[tmp_size] = '\0';
		debug_print(tmp);
		tmp = (char*)realloc(tmp,sizeof(char)*(tmp_size+1));
		res = tmp;
                return index;
	}
	else{
		index++;
		index+=get_token(&string[index],res);
		return index;
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
int set_elements(const char *string,_pl* root, int index, int *flag, int *p);
// index는 끊겼다가 다시 시작할 인덱스, flag는 true일 때 중간에 끊겼다는 뜻,p는 끊긴 부분 인덱스
int list_elements(const char *string,_pl* root, int index, int *flag, int *p){
	int i = 0;
	char *tok;
	if(root->size>0){
		char* type = root->array[root->size-1]->type;
		if(str_cmp(type,"primitive")){
			if((root->size==1)&&(root->array[0]->data==null)){
				i+=get_token(&string[i],tok);
				if(tok[0]=='('){
                        		_pl *set = (_pl*)malloc(sizeof(_pl));
                        		set->type = "set";
                        		i+=set_elements(&string[i],set,index/(root->size+1),flag,p);
                        		if(*flag){	
						(*p) = ((*p)* (root->size+1))+root->size;
                                		return i;
                       			}
                		}else if(tok[0]=='['){
                        		_pl *list = (_pl*)malloc(sizeof(_pl));
                        		list->type = "list";
                        		i+=list_elements(&string[i],list,index/(root->size+1),flag,p);
					if(*flag){	
						(*p) = ((*p)* (root->size+1))+root->size;
                                		return i;
                       			}
                		}else if(tok[0]==']'){
					//error
				}else if(tok[0]==')'){
					//error
				}else{		
                        		root->array[0]->type = "primitive";
					root->array[0]->data=tok;	
                		}
			}else if(!is_blank(*string)) {
				i+=get_token(&string[i],tok);
				if(tok[0]=='('){
					//error
                		}else if(tok[0]=='['){
					//error
                		}else if(tok[0]==']'){
					//error
				}else if(tok[0]==')'){
					return i;
				}else{		
					root->array[root->size-1]->data = str_sum(root->array[root->size-1]->data,tok);
                		}
			}
		}else if(str_cmp(type,"set")){
			i+=set_elements(string,root->array[root->size-1],index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
                                return i;
			}
		}else if(str_cmp(type,"list")){
			i+=list_elements(string,root->array[root->size-1],index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
                                return i;
			}
		}
	}
	while(1){
		i+=get_token(&string[i],tok);
		if(tok[0]=='('){
			_pl *set = (_pl*)malloc(sizeof(_pl));
			set->type = "set";
			
			i+=set_elements(&string[i],set,index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
				return i;
			}
		}else if(tok[0]=='['){
			_pl *list = (_pl*)malloc(sizeof(_pl));
			list->type = "list";
			i+=list_elements(&string[i],list,index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
				return i;
			}
		}else if(tok[0]==']'){
			//error
		}else if(tok==null){
			if(root->size==0){
				_pl *prim = (_pl*)malloc(sizeof(_pl));
				prim->type = "primitive";
				prim->data = null;
				root->size++;
			}
			*p = root->size; 
			*flag = 1;
			return i;
		}else if(tok[0]==')'){
			return i;
		}
		else{
			_pl *prim = (_pl*)malloc(sizeof(_pl));
			prim->type = "primitive";
			prim->data = tok;
		}
		root->size++;
	}

}

// index는 끊겼다가 다시 시작할 인덱스, flag는 true일 때 중간에 끊겼다는 뜻,p는 끊긴 부분 인덱스
int set_elements(const char *string,_pl* root, int index, int *flag, int *p){
	int i = 0;
	char *tok;
	if(root->size>0){
		char* type = root->array[root->size-1]->type;
		if(str_cmp(type,"primitive")){
			if((root->size==1)&&(root->array[0]->data==null)){
				i+=get_token(&string[i],tok);
				if(tok[0]=='('){
                        		_pl *set = (_pl*)malloc(sizeof(_pl));
                        		set->type = "set";
                        		i+=set_elements(&string[i],set,index/(root->size+1),flag,p);
                        		if(*flag){	
						(*p) = ((*p)* (root->size+1))+root->size;
                                		return i;
                       			}
                		}else if(tok[0]=='['){
                        		_pl *list = (_pl*)malloc(sizeof(_pl));
                        		list->type = "list";
                        		i+=list_elements(&string[i],list,index/(root->size+1),flag,p);
					if(*flag){	
						(*p) = ((*p)* (root->size+1))+root->size;
                                		return i;
                       			}
                		}else if(tok[0]==']'){
					//error
				}else if(tok[0]==')'){
					//error
				}else{		
                        		root->array[0]->type = "primitive";
					root->array[0]->data=tok;	
                		}
			}else if(!is_blank(*string)) {
				i+=get_token(&string[i],tok);
				if(tok[0]=='('){
					//error
                		}else if(tok[0]=='['){
					//error
                		}else if(tok[0]==']'){
					//error
				}else if(tok[0]==')'){
					return i;
				}else{		
					root->array[root->size-1]->data = str_sum(root->array[root->size-1]->data,tok);
                		}
			}
		}else if(str_cmp(type,"set")){
			i+=set_elements(string,root->array[root->size-1],index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
                                return i;
			}
		}else if(str_cmp(type,"list")){
			i+=list_elements(string,root->array[root->size-1],index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
                                return i;
			}
		}
	}
	while(1){
		i+=get_token(&string[i],tok);
		if(tok[0]=='('){
			_pl *set = (_pl*)malloc(sizeof(_pl));
			set->type = "set";
			
			i+=set_elements(&string[i],set,index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
				return i;
			}
		}else if(tok[0]=='['){
			_pl *list = (_pl*)malloc(sizeof(_pl));
			list->type = "list";
			i+=list_elements(&string[i],list,index/(root->size+1),flag,p);
			if(*flag){
				(*p) = ((*p)* (root->size+1))+root->size;
				return i;
			}
		}else if(tok[0]==']'){
			//error
		}else if(tok==null){
			if(root->size==0){
				_pl *prim = (_pl*)malloc(sizeof(_pl));
				prim->type = "primitive";
				prim->data = null;
				root->size++;
			}
			*p = root->size; 
			*flag = 1;
			return i;
		}else if(tok[0]==')'){
			return i;
		}
		else{
			_pl *prim = (_pl*)malloc(sizeof(_pl));
			prim->type = "primitive";
			prim->data = tok;
		}
		root->size++;
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

// 중간에 끊겨도 다시 파싱할 수 있게 만들 함수
// TODO : 스트링 토큰 소모할 때 변수로 인덱스를 지정하나 포인터 덧셈 연산으로 하나 -> 함수 내에서 포인터 덧셈하면 외부에서 변하나
int parse_start(const char *string,_pl *root,int index){
	int i=0;
	if(index==0){
		char *tok;
		i += get_token(string,tok);
		//while(tok!=null){
			if(tok[0]=='('){
				root->type = "set";
				int flag = 0;
				int last =0;
				i+=set_elements(string,root,index,&flag,&last);
				if(flag) return last;
			}else if(tok[0]=='['){
				root->type = "list";
				int flag = 0;
				int last =0;
				i+=list_elements(string,root,index,&flag,&last);
				if(flag) return last;
			}else{
				root->type = "primitive";
			}
		//}
	}else{
		if(!str_cmp(root->type,"primitive")){
			// 셋이나 리스트일 경우 depth를 높여감
			char* type = root->array[root->size-1]->type;
			if(str_cmp(type,"set")){
				int flag = 0;
				int last = 0;
				i += set_elements(string,root,index/(root->size+1),&flag,&last);
				if(flag){
					return last;
				}
			}else if(str_cmp(type,"list")){
				int flag = 0;
				int last = 0;
				i += set_elements(string,root,index/(root->size+1),&flag,&last);
				if(flag){
					return last;
				}
			}
		}else{
			// 스트링일 경우
			char *tok;
			i+=get_token(string,tok);
			if(tok[0]=='('){
				root->type = "set";
				int flag = 0;
				int last = 0;
				i+=set_elements(string,root,index/(root->size+1),&flag,&last);
			}else if(tok[0]=='['){
				root->type = "list";
				int flag = 0;
				int last = 0;
				i+=list_elements(string,root,index/(root->size+1),&flag,&last);
			}else{
				root->type = "primitive";
			}
		}
		_pl *pl_index = pl_get(root,index);
	}
}
void parse_instruction(char *string,_parser *parser){
	parser->index = 0;
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
//TODO : debugger main thread
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
