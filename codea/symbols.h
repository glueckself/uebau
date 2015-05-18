#ifndef __SYMBOL_TABLE
#define __SYMBOL_TABLE

typedef struct symbol_t symbol_t;

typedef enum {
    TYPE_UNKNOWN=0,
    TYPE_NUM,
    TYPE_LIST,
    TYPE_CLOSURE
} eSymType;

struct symbol_t {
	symbol_t *next;
	char *name;
        eSymType type;
        const char *regname;
};

void checkKnownSymbol(symbol_t *list, char *name);
void checkUnknownSymbol(symbol_t *list, char *name);
void checkUnresolvedList(void);
symbol_t* addSymbol(symbol_t *list, char *name);
symbol_t* addGlobalSymbol(symbol_t *list, char *name);
symbol_t* mergeLists(symbol_t *list1, symbol_t *list2);

symbol_t* lookupSymbol(symbol_t *list, char *name);

void addSymbolStorage(symbol_t *list, char *name, const char *reg);

#define mkList(name) addSymbol(NULL, name)
#define copyList(list) mergeLists(NULL, list)

#endif
