#ifndef __ASMGEN_H
#define __ASMGEN_H

void genAdd(const char *dstReg, const char *srcReg);
void genMinus(const char *dstReg, const char *srcReg);
void genMult(const char *dstReg, const char *srcReg);
void genAnd(const char *dstReg, const char *srcReg);
void genNot(const char *dstReg);
void genLess(const char *dstReg, const char *srcReg);
void genEqual(const char *dstReg, const char *srcReg);
void genIsNum(const char *dstReg);
void genIsList(const char *dstReg);
void genIsFun(const char *dstReg);
void genDot(const char *dstReg, const char *srcReg);
void genHead(const char *dstReg);
void genTail(const char *dstReg);
void assignFromIdent(bnode->regname, bnode->ident);
void assignFromNum(bnode->regname, bnode->value);

#endif
