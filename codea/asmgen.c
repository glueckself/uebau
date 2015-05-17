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
    //printf("sar $2 %%%s\n", reg);
}

void tagNum(const char *reg) {
    //printf("sal $2 %%%s\n", reg);
}

void extractList(const char *reg) {
}

void tagList(const char *reg) {
}

void genSymbol(const char *fName) {
    printf(".text\n");
    printf(".globl %s\n", fName);
    printf("\t.type %s, @function\n", fName);
    printf("%s:\n",fName);
}

void genReturn(const char *srcReg) {
    printf("ret");
}

void genAdd(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("add %%%s, %%%s\n", srcReg, dstReg);
    tagNum(dstReg);
}

void genAddI(const char *dstReg, const long value) {
    extractNum(dstReg);
    printf("add $%d, %%%s\n", value, dstReg);
    tagNum(dstReg);
}

void genMinus(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("sub %%%s, %%%s\n", srcReg, dstReg);
    tagNum(dstReg);
}

void genMinusI(const char *dstReg, const long value) {
    extractNum(dstReg);
    printf("sub $%d, %%%s\n", value, dstReg);
    tagNum(dstReg);
}
    
void genMult(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("imul %%%s, %%%s\n", srcReg, dstReg);
    tagNum(dstReg);
}

void genMultI(const char *dstReg, const long value) {
    extractNum(dstReg);
    printf("imul $%d, %%%s\n", value, dstReg);
    tagNum(dstReg);
}

void genAnd(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("and %%%s, %%%s\n", srcReg, dstReg);
    tagNum(dstReg);
}

void genAndI(const char *dstReg, const long value) {
    extractNum(dstReg);
    printf("and $%d, %%%s\n", value, dstReg);
    tagNum(dstReg);
}

void genNot(const char *dstReg) {
    extractNum(dstReg);
    printf("neg %%%s\n", dstReg);
    tagNum(dstReg);
}

void genLess(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
}

void genEqual(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
}

void genIsNum(const char *dstReg) {
}

void genIsList(const char *dstReg) {
}

void genIsFun(const char *dstReg) {
}

void genDot(const char *dstReg, const char *srcReg) {
}

void genHead(const char *dstReg) {
}

void genTail(const char *dstReg) {
}

void assignFromIdent(const char *dstReg, const char *srcReg ) {
    printf("mov %%%s, %%%s\n", srcReg, dstReg);
}

void assignFromNum(const char *reg, long value) {
    printf("mov $%d, %%%s\n", value, reg);
    tagNum(reg);
}


