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
    | Term
    | { NOT | HEAD | TAIL | ISNUM | ISLIST | ISFUN } Term
    | Term { PLUS Term }
    | Term MINUS Term
    | Term { MULT Term }
    | Term { AND Term }
    | Term { DOT Term }
    | Term LESS Term | Term EQUAL Term
    | Expr Term /* Funktionsaufruf */
    ;

Term: LBRACKET Expr RBRACKET
    | NUM
    | IDENT
    ;
