%{
#include <stdio.h>
#include <stdlib.h>
%}

%union {
	unsigned int num;
	char *ident;
}

%token <num> NUM
%token <ident> IDENT
%token AND
%token END
%token ISNUM
%token ISLIST
%token ISFUN
%token FUN
%token IF
%token THEN
%token ELSE
%token LET
%token IN
%token NOT
%token HEAD
%token TAIL
%token SEMICOLON
%token EQUAL
%token PLUS
%token MINUS
%token MULT
%token DOT
%token LESS
%token LBRACKET
%token RBRACKET
%token ARROW

%%
Program: Def SEMICOLON 
       ;

Def: IDENT EQUAL Lambda
   ;

Lambda:	FUN IDENT ARROW Expr END
      ;

Expr: IF Expr THEN Expr ELSE Expr END
    | Lambda
    | LET IDENT EQUAL Expr IN Expr END
    | EExpr
    | AddTerm
    | MultTerm
    | AndTerm
    | DotTerm
    | Term MINUS Term
    | Term LESS Term
    | Term EQUAL Term
    | Expr Term /* Funktionsaufruf */
    ;

EExpr: Term
     | KExpr
     ;

KExpr: NOT EExpr
     | HEAD EExpr
     | TAIL EExpr
     | ISNUM EExpr
     | ISLIST EExpr
     | ISFUN EExpr
     ;

AddTerm: Term PLUS Term
       | AddTerm PLUS Term
       ;

MultTerm: Term MULT Term
        | MultTerm MULT Term
        ;

AndTerm: Term AND Term
       | AndTerm AND Term
       ;

DotTerm: Term DOT Term
       | DotTerm DOT Term;
       ;

Term: LBRACKET Expr RBRACKET
    | NUM
    | IDENT
    ;
