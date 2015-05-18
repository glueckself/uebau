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
    printf("mov $3, %r12\n");
    printf("and %%%s, %r12\n", reg);
    printf("cmp $1, %r12\n");
    printf("jne raisesig\n");
    printf("sub $1, %%%s\n", reg);
}

void tagList(const char *reg) {
    printf("add $1, %%%s\n", reg);
}

void genSymbol(const char *fName) {
    printf("\n.text\n");
    printf(".globl %s\n", fName);
    printf("\t.type %s, @function\n", fName);
    printf("%s:\n",fName);
    printf("enter\n");
    printf("pushq %r12\n");
}

void genReturn(const char *srcReg) {
    printf("pop %r12\n");
    printf("leave\n");
    printf("ret\n\n");
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
    printf("not %%%s\n", dstReg);
}

void genLess(const char *dstReg, const char *srcReg) {
    printf("cmp %%%s, %%%s\n", srcReg, dstReg);
    printf("setb %%%s\n", dstReg);
}

void genEqual(const char *dstReg, const char *srcReg) {
    printf("cmp %%%s, %%%s\n", srcReg, dstReg);
    printf("sete %%%s", dstReg);
}

void genIsNum(const char *dstReg) {
    printf("sar $1, %%%s\n", dstReg);
    printf("setnc %%%s\n", dstReg);
}

void genIsList(const char *dstReg) {
    printf("mov $3, %r12\n");
    printf("and %%%s, %r12\n", dstReg);
    printf("cmp $1, %r12\n");
    printf("sete %%%s\n", dstReg);
}

void genIsFun(const char *dstReg) {
    printf("test $3, %%%s\n", dstReg);
    printf("sete %%%s\n", dstReg);
}

const char* genDot(const char *dstReg, const char *srcReg) {
    printf("mov %%%s, 0(%r15)\n", dstReg);
    printf("mov %%%s, 8(%r15)\n", srcReg);
    printf("mov %r15, %%%s\n", dstReg);
    printf("add $16, %r15\n");
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


