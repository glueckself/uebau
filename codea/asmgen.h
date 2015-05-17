#ifndef __ASMGEN_H
#define __ASMGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef CODE
typedef struct burm_state *STATEPTR_TYPE;
#endif

#define OP_LABEL(p)     	((p)->op)
#define LEFT_CHILD(p)   	((p)->kids[0])
#define RIGHT_CHILD(p)  	((p)->kids[1])
#define STATE_LABEL(p)  	((p)->state)
#define STATE_TYPE              STATEPTR_TYPE
#define NODEPTR_TYPE            NODEPTR_TYPE
#define PANIC			printf

typedef enum {
    OP_PLUS=1,
    OP_MINUS=2,
    OP_MULT=3,
    OP_AND=4,
    OP_NOT=5,
    OP_LESS=6,
    OP_EQUAL=7,
    OP_ISNUM=8,
    OP_ISLIST=9,
    OP_ISFUN=10,
    OP_DOT=11,
    OP_HEAD=12,
    OP_TAIL=13,
    OP_CALL=14,
    OP_IDENT=15,
    OP_NUM=16
} eOp;

typedef struct _NodeType *NODEPTR_TYPE;
typedef struct _NodeType NODE_TYPE;
struct _NodeType {
    NODEPTR_TYPE kids[2];
    eOp op;
    STATEPTR_TYPE state;
    int val;
    char *ident;
    
    const char *regname;
};

NODEPTR_TYPE newNode(eOp op, NODEPTR_TYPE left, NODEPTR_TYPE right);
NODEPTR_TYPE newNumNode(int value);
NODEPTR_TYPE newIdentNode(char *storage);
const char* getNextReg(const char *reg);    

void genSymbol(const char *fName);
void genReturn(const char *srcReg);

void genAdd(const char *dstReg, const char *srcReg);
void genAddI(const char *dstReg, const long value);

void genMinus(const char *dstReg, const char *srcReg);
void genMinusI(const char *dstReg, const long value);

void genMult(const char *dstReg, const char *srcReg);
void genMultI(const char *dstReg, const long value) ;

void genAnd(const char *dstReg, const char *srcReg);
void genAndI(const char *dstReg, const long value);

void genNot(const char *dstReg);
void genLess(const char *dstReg, const char *srcReg);
void genEqual(const char *dstReg, const char *srcReg);
void genIsNum(const char *dstReg);
void genIsList(const char *dstReg);
void genIsFun(const char *dstReg);
void genDot(const char *dstReg, const char *srcReg);
void genHead(const char *dstReg);
void genTail(const char *dstReg);
void assignFromIdent(const char *dstReg, const char *srcReg );
void assignFromNum(const char *reg, long value);

#endif
