#include "picolisp.h"
#include "util/map.h"

#define lazy-first 'z'
#define READ_STR	"read"
#define WRITE_STR	"write"
#define ADD_STR		"add"
#define SUB_STR		"sub"
#define READ	'r'
#define WRITE	'w'
#define ADD	'a'
#define SUB	's'

#define REG1_STR	"$reg1"
#define REG2_STR	"$reg2"
#define REG3_STR	"$reg3"
#define REG4_STR	"$reg4"
#define REG1	'1'
#define REG2	'2'
#define REG3	'3'
#define REG4	'4'

//hard-wire configuation
#define MYID 0

typedef struct Wire{
	long wire;
	//|sig_to_rom(1bit)|data_from_rom(sizeof(_sci32)|....|
} _wire_table;

typedef struct Register{
	long a;
	long b;
	long c;
	long d;
} _reg_table;
typedef struct SeudoCPUInstruction{
	char type;  //1byte
	union{
		struct{
			char reg_dst;
			char reg_arg1;
			char reg_arg2;
		};
		struct{
			char reg;
			int addr;
		};
	};
} _sci32;
typedef struct SeudoCPUInstruction64{
	char type;  //1byte
        union{          
                struct{
                        char reg_dst;
                        char reg_arg1;
                        char reg_arg2;
                };
                struct{
                        char reg;
                        long addr;
                };
        };	
} _sci64;

char eval_reg(char* string){
	SWITCH_STRING(string)
		CASE(REG1_STR)
			return REG1;
		CASE(REG2_STR)
			return REG2;
		CASE(REG3_STR)
			return REG3;
		CASE(REG4_STR)
			return REG4;
		DEFAULT()
			break;
	END_SWITCH()
	return '0';
}

long eval_hex(char* string){
	bool pre = (string[0]=='0')&&(string[1]=='1');
	if(!pre){
		return 0;	
	}
	int i=2;
	long hex = 0;
	while(string[i]!='/0'){
		hex<<4;
		switch(string[i]){
			case '0':
			hex+=0;
			break;
			case '1':
			hex+=1;
			break;
			case '2':
			hex+=2;
			break;
			case '3':
			hex+=3;
			break;
			case '4':
			hex+=4;
			break;
			case '5':
			hex+=5;
			break;
			case '6':
			hex+=6;
			break;
			case '7':
			hex+=7;
			break;
			case '8':
			hex+=8;
			break;
			case '9':
			hex+=9;
			break;
			case 'a':
			hex+=10;
			break;
			case 'b':
			hex+=11;
			break;
			case 'c':
			hex+=12;
			break;
			case 'd':
			hex+=13;
			break;
			case 'e':
			hex+=14;
			break;
			case 'f':
			hex+=15;
			break;
			default:
			return 0;
		}
		i++;
	}
	return hex;
}

int check_order(_pl* pl){
	if(pl->type=="primitive") return 0;
	else if(pl->type=="set") return check_order(pl->array[0])+1;	
	else{
		debug_print("error picolist type");
		return 0;
	}
}

void inst_alloc(_pl* pl, _sci64* loc){
	debug_print(pl->type);
	int order = check_order(pl);
	if(order>1) {
		debug_print("error order");
		return;
	}else if(pl->type=="set"){
	SWITCH_STRING(pl->array[0]->data)
		CASE("read")
			loc->type =READ;
			debug_print("read");
			loc->reg = eval_reg(pl->array[1]->data);
			loc->addr = eval_hex(pl->array[2]->data);
			break;
		CASE("write")
			loc->type =WRITE;
			debug_print("write");
			loc->reg = eval_reg(pl->array[1]->data);
			loc->addr = eval_hex(pl->array[2]->data);
			break;
		CASE("add")
			loc->type=ADD;
			loc->reg_dst = eval_reg(pl->array[1]->data);
			loc->reg_arg1 = eval_reg(pl->array[2]->data);
			loc->reg_arg2 = eval_reg(pl->array[3]->data);
			break;
		CASE("sub")
			loc->type=SUB;
			loc->reg_dst = eval_reg(pl->array[1]->data);
			loc->reg_arg1 = eval_reg(pl->array[2]->data);
			loc->reg_arg2 = eval_reg(pl->array[3]->data);
			break;
		DEFAULT()
			break;
	END_SWITCH()
	}else if(pl->type=="primitive"){

	}
}

_sci32* bytecode_gen(_parser* parser, void* memory){

}

_sci64* bytecode_gen64(_parser* parser, void* memory){
	_sci64 *inst_space = (_sci64*)memory;
	for(int i=0;i<parser->array_size;i++){
		inst_alloc(parser->instruction_array[i], &inst_space[i]);
	}
}



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
	void *memory = (void*)malloc(sizeof(_sci64)*1000);
	bytecode_gen64(&parser,memory);

        printf("hello\n");
        return 0;
}
