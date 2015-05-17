#include <stdio.h>

#include "asmgen.h"


void extractNum(const char *reg) {
}

void tagNum(const char *reg) {
}

void extractList(const char *reg) {
}

void tagList(const char *reg) {
}

void genAdd(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("add %s, %s\n", srcReg, dstReg);
}

void genMinus(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("sub %s, %s\n", srcReg, dstReg);
}

void genMult(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("mult %s, %s\n", srcReg, dstReg);
}

void genAnd(const char *dstReg, const char *srcReg) {
    extractNum(srcReg);
    extractNum(dstReg);
    printf("and %s, %s\n", srcReg, dstReg);
}

void genNot(const char *dstReg) {
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

void assignFromIdent(const char *reg, long value) {
}

void assignFromNum(const char *reg, long value) {
}


