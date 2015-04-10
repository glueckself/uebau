#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "symbols.h"

#ifdef DEBUG
#define dbg(text) debugPrint(list, name, text)
#define dbgsect(text) printf("("text")")
#else
#define dbg(...)
#define dbgsect(text) 
#endif

static void debugPrint(symbol_t *list, char *name, char *text) {
    symbol_t *item;
    
    printf("[DBG] %s: %s\n\tList: ", name, text);
    for(item=list; item != NULL; item++)
        printf("%s -> ", item->name);
    printf("X\n");
}
        

static void symbolError(char *text, char *name) {
    printf("Error: %s: %s\n", name, text);
    exit(3);
}

static symbol_t* mkElement(char *name) {
    symbol_t *item;
    
    item = (symbol_t*)calloc(1,sizeof(symbol_t));
    item->name = name;
    
    return item;
}

static symbol_t* lookupSymbol(symbol_t *list, char *name) {
    symbol_t *item=NULL;
    
    if(list == NULL)
        return NULL;
    
    dbg("lookup");
    
    for(item=list; item != NULL; item=item->next) {
        if(strcmp(name, item->name) == 0)
            return item;
    }
    
    return NULL;
}

symbol_t* addSymbol(symbol_t *list, char *name) {
    symbol_t *item;
    
    if(lookupSymbol(list, name) != NULL)
        symbolError("symbol already exists", name);
    
    item=mkElement(name);
    
    item->next=list;
    dbg("added symbol");
    
    return item;
}

symbol_t* addGlobalSymbol(symbol_t *list, char *name) {
    symbol_t *item;
    
    if(lookupSymbol(list, name) != NULL)
        symbolError("global symbol already exists", name);
    
    for(item=list; item->next != NULL; item=item->next); //beim letzten element stehen bleiben
    
    dbg("added global symbol");
    item->next=mkElement(name);
    
    return list;
}

void checkKnownSymbol(symbol_t *list, char *name) {
    if(lookupSymbol(list, name) == NULL)
        symbolError("undefined symbol", name);
}

void checkUnknownSymbol(symbol_t* symlist, char* name) {
    if(lookupSymbol(symlist, name) != NULL)
        symbolError("symbol already defined", name);
}

symbol_t* mergeLists(symbol_t *list1, symbol_t *list2) {
    symbol_t *item;
    
    dbgsect("merge");
    
    for(item=list2; item != NULL; item = item->next) {
        list1=addSymbol(list1, item->name);
    }
    
    dbgsect("end merge");
    
    return list1;
}
