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
Program: Def SEMICOLON  { printf("Prog Def\n"); }
       | Program Def SEMICOLON { printf("Next Program\n"); }
       ;

Def: IDENT EQUAL Lambda { printf("Definition\n"); }
   ;

Lambda:	FUN IDENT ARROW Expr END {printf("Lambda\n");}
      ;

Expr: IF Expr THEN Expr ELSE Expr END { printf("ExprIf\n"); }
    | Lambda {printf("ExprLambda\n");}
    | LET IDENT EQUAL Expr IN Expr END {printf("ExprLet\n");}
    | EExpr {printf("ExprEExpr\n");}
    | AddTerm {printf("ExprAddTerm\n");}
    | MultTerm {printf("ExprMultTerm\n");}
    | AndTerm {printf("ExprAndTerm\n");}
    | DotTerm {printf("ExprDotTerm\n");}
    | Term MINUS Term {printf("ExprMinusTerm\n");}
    | Term LESS Term {printf("ExprLessTerm\n");}
    | Term EQUAL Term {printf("ExprEqualTerm\n");}
    | Expr Term {printf("ExprFuncTerm\n");} /* Funktionsaufruf */
    ;

EExpr: Term {printf("EExprTerm\n");}
     | KExpr {printf("EExprKExpr\n");}
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

Term: LBRACKET Expr RBRACKET {printf("TermExpr\n");}
    | NUM { printf("Number: %d\n", $1); }
    | IDENT { printf("Ident: %s\n", $1); }
    ;
