#ifndef __ASMGEN_H
#define __ASMGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "symbols.h"

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
    OP_NUM=16,
    OP_EVAL=17,
    OP_IF=18,
    OP_CONDEVAL=19,
    OP_LET=20,
    OP_CLOSURE=21
} eOp;

typedef enum {
  REG_FREE=0,
  REG_USED,
} eRegState;

typedef struct {
    char *name;
    char *byteName;
    symbol_t *ident;
    eRegState state;

 //   eSymType type;
} sRegister;

typedef struct _NodeType *NODEPTR_TYPE;
typedef struct _NodeType NODE_TYPE;
struct _NodeType {
    NODEPTR_TYPE kids[2];
    NODEPTR_TYPE parent;
    eOp op;
    STATEPTR_TYPE state;
    int labelNum;
    int val;
    symbol_t *ident;
    const char *regname;
    const char *name;
    sRegister *regList;
};


NODEPTR_TYPE newNode(eOp op, NODEPTR_TYPE left, NODEPTR_TYPE right);
NODEPTR_TYPE newNumNode(int value);
NODEPTR_TYPE newIdentNode(symbol_t *ident);
const char* getNextReg(sRegister *list, const char *regName);
const char* getResultReg();
const char* getParamReg();
void markReg(sRegister *list, const char *regName, eRegState state);

#define SINGLE_ASSIGN(list, dst, src) do {markReg((list), (dst), REG_USED); markReg((list),(src),REG_FREE);}while(0)
#define DOUBLE_ASSIGN(list, dst, src1, src2) do{ markReg((list), (dst), REG_USED); markReg((list),(src1),REG_FREE); markReg((list),(src2),REG_FREE);}while(0)

void assignIdentToReg(sRegister *list, const char *reg, symbol_t *ident);

sRegister* newRegList(void);
int nextIfLabelNum(void);
const char* labelNameFromNum(const char *prefix, int num);

void genNumFromIdent(const char *regname, symbol_t *sym);
void genNumFromReg(const char *dstReg, const char *srcReg);
void genTagNum(const char *dstReg, const char *srcReg);
void genListFromIdent(const char *regname, symbol_t *sym);
void genListFromReg(const char *dstReg, const char *srcReg);
void genTagList(const char *dstReg, const char *srcReg);
void genSymbol(const char *fName);
void genLabel(const char *fName);
void genReturn(const char *dstReg, const char *srcReg);
void genAdd(const char *dstReg, const char *srcReg1, const char *srcReg2);
void genAddI(const char *dstReg, const char *srcReg, const long value);
void genMinus(const char *dstReg, const char *srcReg1, const char *srcReg2);
void genMinusI(const char *dstReg, const char *srcReg, const long value);
void genMult(const char *dstReg, const char *srcReg1, const char *srcReg2);
void genMultI(const char *dstReg, const char *srcReg, const long value);
void genAnd(const char *dstReg, const char *srcReg1, const char *srcReg2);
void genAndI(const char *dstReg, const char *srcReg, const long value);
void genNot(const char *dstReg, const char *srcReg);
void genLess(const char *dstReg, const char *srcReg1, const char *srcReg2);
void genEqual(const char *dstReg, const char *srcReg1, const char *srcReg2);
void genIsNum(const char *dstReg, const char *srcReg);
void genIsList(const char *dstReg, const char *srcReg);
void genIsFun(const char *dstReg, const char *srcReg);
void genDot(const char *dstReg, const char *srcReg, const char *srcReg2);
void genHead(const char *dstReg, const char *srcReg);
void genTail(const char *dstReg, const char *srcReg);
void assignFromIdent(const char *dstReg, const char *srcReg );
void assignFromNum(const char *reg, long value);

void genIf(const char *reg, int labelNum);
void genElseLabel(const char *reg, int labelNum);
void genEndIfLabel(const char *reg, int labelNum);

void genClosure(const char *dstReg, const char *label, symbol_t *symbols);
void genTagFunc(const char *dstReg, const char *srcReg);
void restoreEnvironment(sRegister *regList, symbol_t *list);
void genClosureCall(sRegister *regList, const char *dstReg, const char *clsrReg, const char *srcReg);
void genClosureFromReg(const char *dstReg, const char *srcReg);

void genCallSymbol(sRegister *regList, const char *dstReg, const char *symName, const char *srcReg);
#endif
