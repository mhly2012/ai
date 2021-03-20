#include "picolisp.h"
#include "map.h"
#include "table.h"
typedef struct ROM{
	_char_table* const_table;
	
}_rom;

void execute_rom(_rom* rom){

}

void construct_action(_pl* pl, _rom* rom){
	SWITCH_STRING(pl->type)
                CASE("define")
                        if(strcmp(pl->args->type,"arg-list"){
				if(strcmp(pl->args->element->type,"primitive"){
					if(strcmp(pl->args->next->type,"primitive"){
						if(strcmp(pl->args->next->next->type,"end-list"){
i							
						}else debug_print("invalid define");
					}else debug_print("invalid define");
				}else debug_print("invalid define");
			}else debug_print("invalid define");
                        break;
                CASE("INTERRUPT_0")
                        break;
                DEFAULT()
        END_SWITCH()	
}

_rom* construct_rom(_parser* parser){
	_rom* rom;
	for(int i=0;i<parser->array_size;i++){
                construct_action(parser->instruction_array[i], rom);
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
	_rom* rom = construct_rom(&parser);
	execute_rom(rom);
        printf("hello\n");
        return 0;
}
