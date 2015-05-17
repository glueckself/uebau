#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

typedef struct symbol_t symbol_t;
struct symbol_t {
	symbol_t *next;
	char *name;
        char *regname;
};

void checkKnownSymbol(symbol_t *list, char *name);
void checkUnknownSymbol(symbol_t *list, char *name);
void checkUnresolvedList(void);
symbol_t* addSymbol(symbol_t *list, char *name);
symbol_t* addGlobalSymbol(symbol_t *list, char *name);
symbol_t* mergeLists(symbol_t *list1, symbol_t *list2);

void addSymbolStorage(symbol_t *list, char *name, char *reg);
char *getSymbolStorage(symbol_t *list, char *name);

#define mkList(name) addSymbol(NULL, name)
#define copyList(list) mergeLists(NULL, list)

#endif
