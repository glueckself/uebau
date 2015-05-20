#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CODE
#define CODE
#endif
#include "asmgen.h"

#define ENABLE_OPT_TYPECHECK 0

const sRegister regListTemplate[] = {
    {"rax", "al", 0, 0},
    {"rdi", "dil", 0, 0},
    {"r11", "r11b", 0, 0},
    {"r10", "r10b", 0, 0},
    {"r9", "r9b", 0, 0},
    {"r8", "r8b", 0, 0},
    {"rcx", "cl", 0, 0},
    {"rdx", "dl", 0, 0},
    {"rsi", "sil", 0, 0},
    {NULL, 0, 0}
};

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
    sRegister *startPos;
    
    if(name == NULL) {
        startPos=list;
    } else {
        for(i=0; list[i].name != NULL; i++) {
            if(strcmp(list[i].name, name) == 0) {
                startPos=&(list[i+1]);
                break;
            }
        }
        if(startPos == NULL)
            return NULL;
        if(startPos->name == NULL)
            return NULL;
    }
    
    for(i=0; startPos[i].name != NULL; i++) {
        if(startPos[i].isIdent)
            continue;
        
        return &(startPos[i]);
    }
    

    printf("Failed to allocate register, aborting\n");
    exit(4);
    
    return NULL;
}

const char* getNextParamReg(sRegister *list, const char *start) {
    sRegister *reg = _getNextReg(list, start);
    reg->isIdent=1;
    return reg->name;
}

const char* getNextReg(sRegister *list, const char *regName) {
    return _getNextReg(list, regName)->name;
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
    printf("mov $3, %r12\n");
    printf("and %%%s, %r12\n", reg);
    printf("cmp $1, %r12\n");
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

void genSymbol(const char *fName) {
    printf("\n.text\n");
    printf(".globl %s\n", fName);
    printf("\t.type %s, @function\n", fName);
    printf("%s:\n",fName);
    printf("pushq %r12\n");
}

void genReturn(const char *dstReg, const char *srcReg) {
    if(dstReg != NULL && srcReg != NULL)
        move(dstReg, srcReg);
    printf("pop %r12\n");
    printf("ret\n\n");
}

void genAdd(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("add %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genAddI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("add $%d, %%%s\n", value, dstReg);
}

void genMinus(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("sub %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genMinusI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("sub $%d, %%%s\n", value, dstReg);
}
    
void genMult(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("imul %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genMultI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("imul $%d, %%%s\n", value, dstReg);
}

void genAnd(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("and %%%s, %%%s\n", srcReg2, srcReg1);
    move(dstReg, srcReg1);
}

void genAndI(const char *dstReg, const char *srcReg, const long value) {
    move(dstReg, srcReg);
    printf("and $%d, %%%s\n", value, dstReg);
}

void genNot(const char *dstReg, const char *srcReg) {
    move(dstReg, srcReg);
    printf("xor $1, %%%s\n", dstReg);
}

void genLess(const char *dstReg, const char *srcReg1, const char *srcReg2) {
    printf("cmp %%%s, %%%s\n", srcReg2, srcReg1);
    printf("setc %%%s\n", getByteReg(dstReg));
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
    printf("mov $3, %r12\n");
    printf("and %%%s, %r12\n", srcReg);
    printf("cmp $1, %r12\n");
    printf("sete %%%s\n", getByteReg(dstReg));
    printf("and $1, %%%s\n", dstReg);
}

void genIsFun(const char *dstReg, const char *srcReg) {
    printf("test $3, %%%s\n", srcReg);
    printf("sete %%%s\n", getByteReg(dstReg));
    printf("and $1, %%%s\n", dstReg);
}

void genDot(const char *dstReg, const char *srcReg, const char *srcReg2) {
    printf("mov %%%s, 0(%r15)\n", srcReg);
    printf("mov %%%s, 8(%r15)\n", srcReg2);
    printf("mov %r15, %%%s\n", dstReg);
    printf("add $16, %r15\n");
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
    printf("mov $%d, %%%s\n", value, reg);
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
