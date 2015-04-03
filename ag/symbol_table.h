#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

typedef struct symbol_table symbol_table;
struct symbol_table {
	symbol_table *next;
	char *id;
};



#endif
