#include <stdio.h>

#ifndef CODE
#define CODE
#endif
#include "asmgen.h"

#define ENABLE_OPT_TYPECHECK 1

const char *regNames[] = {"rax", "r11", "r10", "r9", "r8", "rcx", "rdx", "rsi", NULL};
const char *reg8Names[] = {"al", "r11b", "r10b", "r9b", "r8b", "cl", "dl", "sil", NULL};
const int numRegs = (sizeof(regNames)/sizeof(char*)) - 1;

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

const char *getNextReg(const char *reg) {
    int i;
    const char *retval;
    
    if(reg == NULL)
        return regNames[0];
    
    for(i=0; i<numRegs; i++) {
        if(strcmp(reg,regNames[i]))
            continue;
        retval=regNames[i+1];
        break;
    }
    
    if(retval == NULL) {
        printf("Failed to allocate register, aborting\n");
        exit(4);
    }
    
    return retval;
}

static const char *getByteReg(const char *reg) {
    int i;
    
    if(reg == NULL)
        return NULL;
    
    for(i=0; regNames[i] != NULL; i++) {
        if(strcmp(reg, regNames[i]) == 0)
            return reg8Names[i];
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
    if(sym->type == TYPE_LIST)
        return;
    sym->type=TYPE_LIST;
#endif
    extractList(sym->regname);
    move(regname, sym->regname);
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
    printf("not %%%s\n", dstReg);
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


