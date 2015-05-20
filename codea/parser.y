%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbols.h"

#ifndef CODE
#define CODE
#endif
#include "asmgen.h"
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
@autosyn globVal
@autosyn node;

@attributes {unsigned int val;} NUM
@attributes {char *val;}        IDENT
@attributes {symbol_t *globSym;} Program
@attributes {symbol_t *globSym; char *globVal; NODEPTR_TYPE node; } Def
@attributes {symbol_t *sym; NODEPTR_TYPE node;} Lambda Expr EExpr KExpr AddTerm MultTerm DotTerm AndTerm Term

@traversal @postorder t
@traversal @preorder reg
@traversal @postorder codegen

%%
File:
    | Program
    ;

Program: Def SEMICOLON @{
             @i @Program.globSym@ = mkList(@Def.globVal@);
             @i @Def.globSym@ = @Program.globSym@;
         @}
/*       | @{ @i @Program.globSym@ = NULL; @}*/
       | Program Def SEMICOLON @{
             @i @Program.globSym@ = addGlobalSymbol(@Program.1.globSym@, @Def.globVal@);
             @i @Def.globSym@ = @Program.globSym@;
         @}
       ;

       //DefIdent überall sichtbar
Def: IDENT EQUAL Lambda @{
        @i @Lambda.sym@ = addSymbol(@Def.globSym@, @IDENT.val@);
        @i @Def.globVal@ = @IDENT.val@;
        @i @Def.node@ = newNode(OP_CALL, @Lambda.node@, NULL);
        
        @t checkGlobalSymbol(@Def.globSym@, @IDENT.val@);
        
        @reg @Lambda.node@->regname = getNextReg(NULL);
        
        @codegen genSymbol(@IDENT.val@);
        @codegen burm_label(@Def.node@); burm_reduce(@Def.node@, 1);
        
     @}
   ;

	//DefIdent in expr sichtbar
Lambda:	FUN IDENT ARROW Expr END @{
            @i @Expr.sym@ = addSymbol(@Lambda.sym@, @IDENT.val@);
            
            @t checkUnknownSymbol(@Lambda.sym@, @IDENT.val@);
            
            @reg addSymbolStorage(@Expr.sym@, @IDENT.val@, "rdi");;
            @reg @Expr.node@->regname = @Lambda.node@->regname;
        @}
      ;

Expr: IF Expr THEN Expr ELSE Expr END @{ @i @Expr.0.node@ = NULL; @} 
    | Lambda
    | LET IDENT EQUAL Expr IN Expr END @{ 
          @t checkUnknownSymbol(@Expr.1.sym@, @IDENT.val@);
          @t checkUnknownSymbol(@Expr.0.sym@, @IDENT.val@); 
          
          @i @Expr.2.sym@ = mergeLists(mergeLists(mkList(@IDENT.val@), @Expr.0.sym@), @Expr.1.sym@);
          @i @Expr.0.node@ = NULL;
      @}
    | EExpr @{
          @i @Expr.node@ = @EExpr.node@;
      @}
    | AddTerm @{
          @i @Expr.node@ = @AddTerm.node@;
      @}
    | MultTerm @{
          @i @Expr.node@ = @MultTerm.node@;
      @}
    | AndTerm @{
          @i @Expr.node@ = @AndTerm.node@;
      @}
    | DotTerm @{
          @i @Expr.node@ = @DotTerm.node@;
      @}
    | Term MINUS Term @{
          @i @Expr.node@ = newNode(OP_MINUS, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.node@->regname);
      @}
    | Term LESS Term @{
          @i @Expr.node@ = newNode(OP_LESS, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.node@->regname);
      @}
    | Term EQUAL Term @{
          @i @Expr.node@ = newNode(OP_EQUAL, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.node@->regname);
      @}
    | Expr Term @{ @i @Expr.0.node@ = NULL; @}
    ;

EExpr: Term @{
          @i @EExpr.node@ = @Term.node@;
       @}
     | KExpr @{
          @i @EExpr.node@ = @KExpr.node@;
       @}
     ;

KExpr: NOT EExpr @{
          @i @KExpr.node@ = newNode(OP_NOT, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
       @}
     | HEAD EExpr @{
          @i @KExpr.node@ = newNode(OP_HEAD, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
       @}
     | TAIL EExpr @{
          @i @KExpr.node@ = newNode(OP_TAIL, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
       @}
     | ISNUM EExpr @{
          @i @KExpr.node@ = newNode(OP_ISNUM, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
       @}
     | ISLIST EExpr @{
          @i @KExpr.node@ = newNode(OP_ISLIST, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
       @}
     | ISFUN EExpr @{
          @i @KExpr.node@ = newNode(OP_ISFUN, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
       @}
     ;

AddTerm: Term PLUS Term @{
            @i @AddTerm.node@ = newNode(OP_PLUS, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @AddTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@AddTerm.node@->regname);
         @}
       | AddTerm PLUS Term @{
            @i @AddTerm.0.node@ = newNode(OP_PLUS, @AddTerm.1.node@, @Term.node@);
            
            @reg @AddTerm.1.node@->regname = @AddTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@AddTerm.0.node@->regname);
         @}
       ;

MultTerm: Term MULT Term @{
            @i @MultTerm.node@ = newNode(OP_MULT, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @MultTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@MultTerm.node@->regname);
          @}
        | MultTerm MULT Term @{
            @i @MultTerm.0.node@ = newNode(OP_MULT, @MultTerm.1.node@, @Term.node@);
            
            @reg @MultTerm.1.node@->regname = @MultTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@MultTerm.0.node@->regname);
          @}
        ;

AndTerm: Term AND Term @{
            @i @AndTerm.node@ = newNode(OP_AND, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @AndTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@AndTerm.node@->regname);
         @}
       | AndTerm AND Term @{
            @i @AndTerm.0.node@ = newNode(OP_AND, @AndTerm.1.node@, @Term.node@);
            
            @reg @AndTerm.1.node@->regname = @AndTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@AndTerm.0.node@->regname);
          @}
       ;

DotTerm: Term DOT Term @{
            @i @DotTerm.node@ = newNode(OP_DOT, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @DotTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@DotTerm.node@->regname);
         @}
       | Term DOT DotTerm @{
            @i @DotTerm.0.node@ = newNode(OP_DOT, @Term.node@, @DotTerm.1.node@);
            
            @reg @DotTerm.1.node@->regname = @DotTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@DotTerm.0.node@->regname);
         @}
       ;

Term: LBRACKET Expr RBRACKET @{ @i @Term.node@ = @Expr.node@; @}
    | NUM @{
        @i @Term.node@ = newNumNode(@NUM.val@);
      @}
    | IDENT @{
        @t checkKnownSymbol(@Term.sym@, @IDENT.val@);
        @i @Term.node@ = newIdentNode(lookupSymbol(@Term.sym@, @IDENT.val@));
      @}
    ;

