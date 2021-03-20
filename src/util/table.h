#define PUT(table,key,value) \
while(table->next!=null){\
	if(strcmp(table->key,key){\
		table->value = value;\
		break;\
	}\
}
#define GET(table,key,res)\
while(table->next!=null){\
	if(strcmp(table->key,key){\
		res = table->value;\
		break;\	
	}\
}
typedef struct CharCharTable _char_table;
typedef struct CharCharTable{
        char* key;
        char* value;
        _char_table* next;
} _char_table;
