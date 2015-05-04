%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbols.h"
%}

%token NUM
%token IDENT

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

@autoinh sym
@autosyn globVal;

@attributes {unsigned int val;} NUM
@attributes {char *val;}        IDENT
@attributes {symbol_t *globSym;} Program
@attributes {symbol_t *globSym; char *globVal;} Def
@attributes {symbol_t *sym;} Lambda Expr EExpr KExpr AddTerm MultTerm DotTerm AndTerm Term

@traversal @postorder t

%%
Program: @{ @i @Program.globSym@ = NULL; @}
       |  Def SEMICOLON @{
             @i @Program.globSym@ = mkList(@Def.globVal@);
             @i @Def.globSym@ = @Program.globSym@;
         @}
       | Program Def SEMICOLON @{
             @i @Program.0.globSym@ = addGlobalSymbol(@Program.1.globSym@, @Def.globVal@);
             @i @Def.globSym@ = @Program.globSym@;
         @}
       ;

       //DefIdent überall sichtbar
Def: IDENT EQUAL Lambda @{
        @i @Lambda.sym@ = addSymbol(@Def.globSym@, @IDENT.val@);
        @i @Def.globVal@ = @IDENT.val@;
     @}
   ;

	//DefIdent in expr sichtbar
Lambda:	FUN IDENT ARROW Expr END @{
            @t checkUnknownSymbol(@Lambda.sym@, @IDENT.val@);
            
            @i @Expr.sym@ = addSymbol(@Lambda.sym@, @IDENT.val@);
        @}
      ;

Expr: IF Expr THEN Expr ELSE Expr END
    | Lambda
    | LET IDENT EQUAL Expr IN Expr END @{ 
          @t checkUnknownSymbol(@Expr.1.sym@, @IDENT.val@);
          @t checkUnknownSymbol(@Expr.0.sym@, @IDENT.val@); 
          
          @i @Expr.2.sym@ = mergeLists(mergeLists(mkList(@IDENT.val@), @Expr.0.sym@), @Expr.1.sym@);
      @}
    | EExpr
    | AddTerm
    | MultTerm
    | AndTerm
    | DotTerm
    | Term MINUS Term
    | Term LESS Term
    | Term EQUAL Term
    | Expr Term
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
    | IDENT @{ @t checkKnownSymbol(@Term.sym@, @IDENT.val@); @}
    ;

