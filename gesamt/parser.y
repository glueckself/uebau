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
@autoinh regList
@autosyn globVal
@autosyn node

@attributes {unsigned int val;} NUM
@attributes {char *val;}        IDENT
@attributes {symbol_t *globSym;} Program
@attributes {symbol_t *globSym; char *globVal; NODEPTR_TYPE node; sRegister *regList;} Def
@attributes {symbol_t *sym; NODEPTR_TYPE node; sRegister *regList; } Lambda Expr EExpr KExpr AddTerm MultTerm DotTerm AndTerm Term

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
             @i @Def.regList@ = newRegList();
         @}
/*       | @{ @i @Program.globSym@ = NULL; @}*/
       | Program Def SEMICOLON @{
             @i @Program.globSym@ = addGlobalSymbol(@Program.1.globSym@, @Def.globVal@);
             @i @Def.globSym@ = @Program.globSym@;
             @i @Def.regList@ = newRegList();
         @}
       ;

       //DefIdent überall sichtbar
Def: IDENT EQUAL Lambda @{
        @i @Lambda.sym@ = addSymbol(@Def.globSym@, @IDENT.val@);
        @i @Def.globVal@ = @IDENT.val@;
        @i @Def.node@ = newNode(OP_EVAL, @Lambda.node@, NULL);
        
        @t checkGlobalSymbol(@Def.globSym@, @IDENT.val@);
        
        @reg @Lambda.node@->regname = getNextReg(@Def.regList@, NULL);
        
        @codegen genSymbol(@IDENT.val@);
        @codegen burm_label(@Def.node@); burm_reduce(@Def.node@, 1);
     @}
   ;

	//DefIdent in expr sichtbar
Lambda:	FUN IDENT ARROW Expr END @{
            @i @Expr.sym@ = addSymbol(@Lambda.sym@, @IDENT.val@);
            
            @t checkUnknownSymbol(@Lambda.sym@, @IDENT.val@);
            
            @reg addSymbolStorage(@Expr.sym@, @IDENT.val@, getNextParamReg(@Lambda.regList@, @Lambda.node@->regname));
            @reg @Expr.node@->regname = @Lambda.node@->regname;
        @}
      ;

Expr: IF Expr THEN Expr ELSE Expr END @{
        @i @Expr.0.node@ = newNode(OP_IF, @Expr.1.node@, newNode(OP_CONDEVAL, @Expr.2.node@, @Expr.3.node@));
        
        @t @Expr.2.node@->parent=@Expr.0.node@;
        @t @Expr.3.node@->parent=@Expr.0.node@;
        
        @reg @Expr.0.node@->labelNum = nextIfLabelNum();
        
        @reg @Expr.1.node@->regname = @Expr.0.node@->regname;
        @reg @Expr.2.node@->regname = @Expr.0.node@->regname;
        @reg @Expr.3.node@->regname = @Expr.0.node@->regname;
        
      @} 
    | Lambda
    | LET IDENT EQUAL Expr IN Expr END @{ 
          @i @Expr.2.sym@ = addSymbol(@Expr.0.sym@, @IDENT.val@);
          @i @Expr.0.node@ = newNode(OP_LET, @Expr.1.node@, @Expr.2.node@);
          
          @t checkUnknownSymbol(@Expr.1.sym@, @IDENT.val@);
          @t checkUnknownSymbol(@Expr.0.sym@, @IDENT.val@); 
          
          @reg @Expr.1.node@->regname = getNextParamReg(@Expr.regList@, @Expr.0.node@->regname); addSymbolStorage(@Expr.2.sym@, @IDENT.val@, @Expr.1.node@->regname);
          @reg @Expr.2.node@->regname = @Expr.0.node@->regname;
      @}
    | EExpr
    | AddTerm
    | MultTerm
    | AndTerm
    | DotTerm
    | Term MINUS Term @{
          @i @Expr.node@ = newNode(OP_MINUS, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.regList@, @Expr.node@->regname);
      @}
    | Term LESS Term @{
          @i @Expr.node@ = newNode(OP_LESS, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.regList@, @Expr.node@->regname);
      @}
    | Term EQUAL Term @{
          @i @Expr.node@ = newNode(OP_EQUAL, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.regList@, @Expr.node@->regname);
      @}
    | Expr Term @{
	  @i @Expr.0.node@ = newNode(OP_CALL, @Term.node@, @Expr.1.node@);
	  @reg @Term.node@->regname = "rdi";
      @}
    ;

EExpr: Term
     | KExpr
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
          @reg @Term.1.node@->regname = getNextReg(@AddTerm.regList@, @AddTerm.node@->regname);
         @}
       | AddTerm PLUS Term @{
            @i @AddTerm.0.node@ = newNode(OP_PLUS, @AddTerm.1.node@, @Term.node@);
            
            @reg @AddTerm.1.node@->regname = @AddTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@AddTerm.regList@, @AddTerm.0.node@->regname);
         @}
       ;

MultTerm: Term MULT Term @{
            @i @MultTerm.node@ = newNode(OP_MULT, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @MultTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@MultTerm.regList@, @MultTerm.node@->regname);
          @}
        | MultTerm MULT Term @{
            @i @MultTerm.0.node@ = newNode(OP_MULT, @MultTerm.1.node@, @Term.node@);
            
            @reg @MultTerm.1.node@->regname = @MultTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@MultTerm.regList@, @MultTerm.0.node@->regname);
          @}
        ;

AndTerm: Term AND Term @{
            @i @AndTerm.node@ = newNode(OP_AND, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @AndTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@AndTerm.regList@, @AndTerm.node@->regname);
         @}
       | AndTerm AND Term @{
            @i @AndTerm.0.node@ = newNode(OP_AND, @AndTerm.1.node@, @Term.node@);
            
            @reg @AndTerm.1.node@->regname = @AndTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@AndTerm.regList@, @AndTerm.0.node@->regname);
          @}
       ;

DotTerm: Term DOT Term @{
            @i @DotTerm.node@ = newNode(OP_DOT, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @DotTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@DotTerm.regList@, @DotTerm.node@->regname);
         @}
       | Term DOT DotTerm @{
            @i @DotTerm.0.node@ = newNode(OP_DOT, @Term.node@, @DotTerm.1.node@);
            
            @reg @DotTerm.1.node@->regname = getNextReg(@DotTerm.regList@, @DotTerm.0.node@->regname);
            @reg @Term.0.node@->regname = (@DotTerm.0.node@->regname);
         @}
       ;

Term: LBRACKET Expr RBRACKET
    | NUM @{
        @i @Term.node@ = newNumNode(@NUM.val@);
      @}
    | IDENT @{
        @t checkKnownSymbol(@Term.sym@, @IDENT.val@);
        @reg if(@Term.node@->ident == NULL) { @Term.node@->ident = lookupSymbol(@Term.sym@, @IDENT.val@);}
        @i @Term.node@ = newIdentNode(lookupSymbol(@Term.sym@, @IDENT.val@));
      @}
    ;

