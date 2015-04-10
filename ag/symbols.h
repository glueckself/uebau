#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

typedef struct symbol_t symbol_t;
struct symbol_t {
	symbol_t *next;
	char *name;
};

void checkKnownSymbol(symbol_t *list, char *name);
void checkUnknownSymbol(symbol_t *list, char *name);

symbol_t* addSymbol(symbol_t *list, char *name);
symbol_t* addGlobalSymbol(symbol_t *list, char *name);
symbol_t* mergeLists(symbol_t *list1, symbol_t *list2);

#define mkList(name) addSymbol(NULL, name)
#define copyList(list) mergeLists(NULL, list)

#endif
