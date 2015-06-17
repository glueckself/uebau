#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "symbols.h"

static symbol_t* mkElement(char *name) {
    symbol_t *item;
    
    item = (symbol_t*)calloc(1,sizeof(symbol_t));
    item->name = name;
    item->type = TYPE_UNKNOWN;
    
    return item;
}

static int getNextFreeOffset(symbol_t *list) {
  symbol_t *e;
  int offset=-8;
  
  if(list == NULL)
    return -1;
  
  for(e=list; e != NULL; e=e->next) {
      if(e->offset == -1)
	continue;
      
      if(e->offset > offset)
	offset=e->offset;
  }
  
  return offset+8;
}

static symbol_t* _addSymbol(symbol_t *list, char *name) {
    symbol_t *item;
    
    item=mkElement(name);
    
    item->next=list;
    
    return item;
}

symbol_t* addSymbol(symbol_t *list, char *name) {
    symbol_t *item=_addSymbol(list, name);

    item->offset=getNextFreeOffset(list);
    
    return item;
}

symbol_t* addGlobalSymbol(symbol_t *list, char *name) {
    symbol_t *item;
    
    for(item=list; item->next != NULL; item=item->next); //beim letzten element stehen bleiben
    
    item->next=mkElement(name);
    item->next->offset=-1;
    
    return list;
}

symbol_t* lookupSymbol(symbol_t *list, char *name) {
    symbol_t *item;
    
    if(list == NULL)
        return NULL;
    
    for(item=list; item != NULL; item=item->next) {
        if(strcmp(name, item->name) == 0)
            return item;
    }
    
    return NULL;
}

void checkKnownSymbol(symbol_t *list, char *name) {
    if(lookupSymbol(list, name) == NULL) {
        printf("Unknown symbol: %s\n", name);
        exit(3);
    }
}

void checkUnknownSymbol(symbol_t* symlist, char *name) {
    if(lookupSymbol(symlist, name) != NULL) {
        printf("Symbol already defined: %s\n", name);
        exit(3);
    }
}

void checkGlobalSymbol(symbol_t *symlist, char *name) {
    symbol_t *item;
    int symCount=0;
    
    if(symlist == NULL)
        return;
    
    for(item=symlist; item != NULL; item=item->next) {
        if(strcmp(name, item->name) == 0)
            symCount++;
    }
    
    if(symCount != 1) {
        printf("Global symbol already defined: %s\n", name);
        exit(3);
    }
}

symbol_t* mergeLists(symbol_t *list1, symbol_t *list2) {
    symbol_t *item;
    
    for(item=list2; item != NULL; item = item->next) {
        list1=_addSymbol(list1, item->name);
	list1->offset=item->offset;
    }
    
    return list1;
}
