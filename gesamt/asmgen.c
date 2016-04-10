#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CODE
#define CODE
#endif
#include "asmgen.h"

#define ENABLE_OPT_TYPECHECK 0

static const symbol_t raxReg = {0};

static const sRegister regListTemplate[] = {
    {"rdi", "dil", (symbol_t*)&raxReg, REG_USED},
    
    {"r11", "r11b", NULL, REG_FREE},
    
    {"r10", "r10b", NULL, REG_FREE},
    {"r9", "r9b", NULL, REG_FREE},
    {"r8", "r8b", NULL, REG_FREE},
    {"rcx", "cl", NULL, REG_FREE},
    {"rdx", "dl", NULL, REG_FREE},
    {"rsi", "sil", NULL, REG_FREE},
};

static const int NUM_REGISTERS = (sizeof(regListTemplate)/sizeof(sRegister));



NODEPTR_TYPE newNode(eOp op, NODEPTR_TYPE left, NODEPTR_TYPE right) {
    NODEPTR_TYPE node = calloc(1, sizeof(NODE_TYPE));
    
    node->op = op;
    node->kids[0]=left;
    node->kids[1]=right;
    
    return node;
}

NODEPTR_TYPE newNumNode(int value) {
    NODEPTR_TYPE node = calloc(1, sizeof(NODE_TYPE));
    
    node->op = OP_NUM;
    node->val = value;
    
    return node;
}

NODEPTR_TYPE newIdentNode(symbol_t *ident) {
    NODEPTR_TYPE node = calloc(1, sizeof(NODE_TYPE));
    
    node->op = OP_IDENT;
    node->ident=ident;
    
    return node;
}

sRegister* newRegList(void) {
    sRegister *list = (sRegister*)malloc(sizeof(regListTemplate));
    memcpy((void*)list, (void*)regListTemplate, sizeof(regListTemplate));
    return list;
}

static sRegister* _getNextReg(sRegister *list, const char *name) {
    int i;
    int startPos=-1;
    
    if(name == NULL) {
        startPos=0;
    } else {
        for(i=0; i < NUM_REGISTERS; i++) {
            if(strcmp(list[i].name, name) == 0) {
                startPos=i+1;
                break;
            }
        }
        if(startPos == -1)
            return &list[0];
    }
    
    for(i=startPos; i<NUM_REGISTERS; i++) {
	if(list[i].ident != NULL)
	    continue;

	if(list[i].state == REG_USED)
	  continue;
        
        return &(list[i]);
    }
    
    //we should compile all statically correct programs but it doesn't mean to execute them correctly :)
    return &list[0];
}


const char* getNextReg(sRegister *list, const char *regName) {
    sRegister *reg = _getNextReg(list, regName);
    return reg->name;
}

const char* getParamReg() {
  return "rdi";
}

const char* getResultReg() {
  return "r11";
}

void markReg(sRegister *list, const char *regName, eRegState state) {
    int i;
    sRegister *reg = NULL;
    
    for(i=0;i<NUM_REGISTERS;i++) {
      if(strcmp(list[i].name, regName) == 0) {
	reg=&(list[i]);
	break;
      }
    }
    
    if(reg == NULL)
      return;
    
    if(reg->ident != NULL)
      state=REG_USED;
    /*
    if(state == REG_USED)
      printf("marking %s as USED\n", regName);
    else
      printf("marking %s as FREE\n", regName);
    */
    reg->state=state;
}
  
void assignIdentToReg(sRegister *list, const char *regName, symbol_t *ident) {
    int i;
    //sRegister *reg;
    
    for(i=0; list[i].name != NULL; i++) {
      if(strcmp(regName, list[i].name) != 0)
	continue;
      
      list[i].ident=ident;
      list[i].state=REG_USED;
      ident->regname=list[i].name;
      return;
    }
    
  //  printf("failed to assign %s to %s\n", ident, regName);
}

static const char *getByteReg(const char *reg) {
    int i;
    
    if(reg == NULL)
        return NULL;
    
    for(i=0; regListTemplate[i].name != NULL; i++) {
        if(strcmp(reg, regListTemplate[i].name) == 0)
            return regListTemplate[i].byteName;
    }
    return NULL;
}

static void saveIdentifiers(sRegister *list) {
    int i;
    
    for(i=0; i<NUM_REGISTERS; i++) {
	if(list[i].state == REG_FREE)
	  continue;
	
	printf("pushq %%%s\n", list[i].name);
    }
}

static void restoreIdentifiers(sRegister *list) {
    int i;
    
    for(i=NUM_REGISTERS-1; i >= 0; i--) {
      if(list[i].state == REG_FREE)
	continue;
      
      printf("popq %%%s\n", list[i].name);
    }
}

static void move(const char *dstReg, const char *srcReg) {
    if(strcmp(dstReg,srcReg) == 0)
        return;
    printf("mov %%%s, %%%s\n", srcReg, dstReg);
}
 
static void extractNum(const char *reg) {
    printf("sar $1, %%%s\n", reg);
    printf("jc raisesig\n");
}

void genNumFromIdent(const char *regname, symbol_t *sym) {
#if ENABLE_OPT_TYPECHECK
    if(sym->type != TYPE_NUM) {
        extractNum(sym->regname);
        sym->type=TYPE_NUM;
    }
#endif

    move(regname, sym->regname);

#if ENABLE_OPT_TYPECHECK == 0
    extractNum(regname);
#endif
}

void genNumFromReg(const char *dstReg, const char *srcReg) {
    move(dstReg, srcReg);
    extractNum(dstReg);
}

void genTagNum(const char *dstReg, const char *srcReg) {
    move(dstReg, srcReg);
    printf("sal $1, %%%s\n", dstReg);
}

static void extractList(const char *reg) {
    printf("mov $3, %%r12\n");
    printf("and %%%s, %%r12\n", reg);
    printf("cmp $1, %%r12\n");
    printf("jne raisesig\n");
    printf("sub $1, %%%s\n", reg);
}

void genListFromIdent(const char *regname, symbol_t *sym) {
#if ENABLE_OPT_TYPECHECK
    if(sym->type != TYPE_LIST) {
        sym->type=TYPE_LIST;
        extractList(sym->regname);
    }
#endif
    
    move(regname, sym->regname);
#if ENABLE_OPT_TYPECHECK == 0
    extractList(regname);
#endif
}

void genListFromReg(const char *dstReg, const char *srcReg) {
    move(dstReg, srcReg);
    extractList(dstReg);
}

void genTagList(const char *dstReg, const char *srcReg) {
    move(dstReg, srcReg);
    printf("add $1, %%%s\n", dstReg);
}


void genTagFunc(const char *dstReg, const char *srcReg) {
    move(dstReg, srcReg);
    printf("add $3, %%%s\n", dstReg);
}

void genSymbol(const char *fName) {
    printf("\n.text\n");
    printf(".globl %s\n", fName);
    printf(".type %s, @function\n", fName);
}

void genLabel(const char *fName) {
    printf("%s:\n",fName);
    printf("pushq %%r12\n");
}

void genReturn(const char *dstReg, const char *srcReg) {
    move("rax", srcReg);
    printf("popq %%r12\n");
    printf("ret\n\n");
}

void genAdd(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("add %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genAddI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("add $%ld, %%%s\n", value, dstReg);
}

void genMinus(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("sub %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genMinusI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("sub $%ld, %%%s\n", value, dstReg);
}
    
void genMult(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("imul %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genMultI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("imul $%ld, %%%s\n", value, dstReg);
}

void genAnd(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("and %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genAndI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("and $%ld, %%%s\n", value, dstReg);
}

void genNot(const char *dstReg, const char *srcReg) {
    move(dstReg, srcReg);
    printf("xor $1, %%%s\n", dstReg);
}

void genLess(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("cmp %%%s, %%%s\n", srcReg2, srcReg1);
    printf("setl %%%s\n", getByteReg(dstReg));
    printf("and $1, %%%s\n", dstReg);
}

void genEqual(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("cmp %%%s, %%%s\n", srcReg1, srcReg2);
    printf("sete %%%s\n", getByteReg(dstReg));
    printf("and $1, %%%s\n", dstReg);
}

void genIsNum(const char *dstReg, const char *srcReg) {
    printf("sar $1, %%%s\n", srcReg);
    printf("setnc %%%s\n", getByteReg(dstReg));
    printf("and $1, %%%s\n", dstReg);
}

void genIsList(const char *dstReg, const char *srcReg) {
    printf("mov $3, %%r12\n");
    printf("and %%%s, %%r12\n", srcReg);
    printf("cmp $1, %%r12\n");
    printf("sete %%%s\n", getByteReg(dstReg));
    printf("and $1, %%%s\n", dstReg);
}

void genIsFun(const char *dstReg, const char *srcReg) {
    printf("test $3, %%%s\n", srcReg);
    printf("sete %%%s\n", getByteReg(dstReg));
    printf("and $1, %%%s\n", dstReg);
}

void genDot(const char *dstReg, const char *srcReg, const char *srcReg2) {
    printf("mov %%%s, 0(%%r15)\n", srcReg);
    printf("mov %%%s, 8(%%r15)\n", srcReg2);
    printf("mov %%r15, %%%s\n", dstReg);
    printf("add $16, %%r15\n");
}

void genHead(const char *dstReg, const char *srcReg) {
     printf("mov 0(%%%s), %%%s\n", srcReg, dstReg);
}

void genTail(const char *dstReg, const char *srcReg) {
     printf("mov 8(%%%s), %%%s\n", srcReg, dstReg);
}

void assignFromIdent(const char *dstReg, const char *srcReg ) {
    move(dstReg, srcReg);
}

void assignFromNum(const char *reg, long value) {
    printf("mov $%ld, %%%s\n", value, reg);
}

void genIf(const char *reg, int labelNum) {
    printf("cmp $0, %%%s\n", reg);
    printf("je elseLb%d\n", labelNum);
}

void genElseLabel(const char *reg, int labelNum) {
    printf("jmp endifLb%d\n", labelNum);
    printf("elseLb%d:\n", labelNum);
}

void genEndIfLabel(const char *reg, int labelNum) {
    printf("endifLb%d:\n", labelNum);
}

int nextIfLabelNum(void) {
    static int labelNum=0;
    return labelNum++;
}

void genCallSymbol(sRegister *regList, const char *dstReg, const char *symName, const char *srcReg) {
    saveIdentifiers(regList);
    move("rdi", srcReg);
    printf("call %s\n", symName);
    move(dstReg, "rax");
    restoreIdentifiers(regList);
}

void genClosure(const char *dstReg, const char *label, symbol_t *symbols) {
    symbol_t *e;
    int symCnt=0;
    
    move(dstReg, "r15");
    
    
    printf("add $16, %%r15\n");
    
    printf("movq $%s, 0(%%%s)\n", label, dstReg);
    printf("mov %%r15, 8(%%%s)\n", dstReg);
    
    for(e=symbols; e != NULL; e=e->next) {
      if(e->offset == -1)
	continue;
      
      printf("mov %%%s, %d(%%r15)\n", e->regname, e->offset);
      symCnt++;
    }
    
    printf("add $%d, %%r15\n", symCnt*8);
}

const char* labelNameFromNum(const char *prefix, int num) {
    char *buf = calloc(100,1);
    
    sprintf(buf, "%s%d", prefix, num);
    
    return buf;
}


void restoreEnvironment(sRegister *regList, symbol_t *list) {
  symbol_t *e;
  const char *reg=NULL;
  
 // printf("restore environment\n");
  
  for(e=list; e != NULL; e=e->next) {
    if(e->offset == -1)
      continue;
    
    reg=getNextReg(regList,reg);
    assignIdentToReg(regList, reg, e);
    
    printf("mov %d(%%r12), %%%s\n", e->offset, e->regname);
  }
}


void genClosureCall(sRegister *regList, const char *dstReg, const char *clsrReg, const char *srcReg) {
    saveIdentifiers(regList);
    move("rdi", srcReg);
    printf("mov 8(%%%s), %%r12\n", clsrReg);
    printf("call *(%%%s)\n", clsrReg);
    move(dstReg, "rax");
    restoreIdentifiers(regList);
}


void genClosureFromReg(const char *dstReg, const char *srcReg) {
  move(dstReg, srcReg);
  printf("sub $3, %%%s\n", dstReg);
}
