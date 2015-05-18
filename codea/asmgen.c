#include <stdio.h>

#ifndef CODE
#define CODE
#endif
#include "asmgen.h"

const char *regNames[] = {"rax", "rdi", "r11", "r10", "r9", "r8", "rcx", "rdx", "rsi", NULL};
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

NODEPTR_TYPE newIdentNode(char *storage) {
    NODEPTR_TYPE node = calloc(1, sizeof(NODE_TYPE));
    
    node->op = OP_IDENT;
    node->regname=storage;
    
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


void extractNum(const char *reg) {
    printf("sar $1, %%%s\n", reg);
    printf("jc raisesig\n");
}

void tagNum(const char *reg) {
    printf("sal $1, %%%s\n", reg);
}

void extractList(const char *reg) {
    printf("test $0x2, %%%s\n", reg);
    printf("je raisesig\n");
    printf("sar $2, %%%s\n", reg);
}

void tagList(const char *reg) {
    printf("sal $2, %%%s\n", reg);
    printf("add $2, %%%s\n", reg);
}

void genSymbol(const char *fName) {
    printf(".text\n");
    printf(".globl %s\n", fName);
    printf("\t.type %s, @function\n", fName);
    printf("%s:\n",fName);
}

void genReturn(const char *srcReg) {
    printf("ret\n");
}

void genAdd(const char *dstReg, const char *srcReg) {
            printf("add %%%s, %%%s\n", srcReg, dstReg);
}

void genAddI(const char *dstReg, const long value) {
        printf("add $%d, %%%s\n", value, dstReg);
}

void genMinus(const char *dstReg, const char *srcReg) {
            printf("sub %%%s, %%%s\n", srcReg, dstReg);
}

void genMinusI(const char *dstReg, const long value) {
        printf("sub $%d, %%%s\n", value, dstReg);
}
    
void genMult(const char *dstReg, const char *srcReg) {
            printf("imul %%%s, %%%s\n", srcReg, dstReg);
}

void genMultI(const char *dstReg, const long value) {
        printf("imul $%d, %%%s\n", value, dstReg);
}

void genAnd(const char *dstReg, const char *srcReg) {
            printf("and %%%s, %%%s\n", srcReg, dstReg);
}

void genAndI(const char *dstReg, const long value) {
        printf("and $%d, %%%s\n", value, dstReg);
}

void genNot(const char *dstReg) {
        printf("neg %%%s\n", dstReg);
}

void genLess(const char *dstReg, const char *srcReg) {
}

void genEqual(const char *dstReg, const char *srcReg) {
}

void genIsNum(const char *dstReg) {
    
}

void genIsList(const char *dstReg) {
}

void genIsFun(const char *dstReg) {
}

const char* genDot(const char *dstReg, const char *srcReg) {

    return dstReg;
}

void genHead(const char *dstReg) {
     printf("mov 0(%%%s), %%%s\n", dstReg, dstReg);
}

void genTail(const char *dstReg) {
     printf("mov 8(%%%s), %%%s\n", dstReg, dstReg);
}

void assignFromIdent(const char *dstReg, const char *srcReg ) {
    if(strcmp(dstReg,srcReg) == 0)
        return;
    printf("mov %%%s, %%%s\n", srcReg, dstReg);
}

void assignFromNum(const char *reg, long value) {
    printf("mov $%d, %%%s\n", value, reg);
}


