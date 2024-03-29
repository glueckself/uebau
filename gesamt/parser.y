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
@attributes {symbol_t *globSym; char *globVal; NODEPTR_TYPE node;} Def
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
         @}
       | Program Def SEMICOLON @{
             @i @Program.globSym@ = addGlobalSymbol(@Program.1.globSym@, @Def.globVal@);
             @i @Def.globSym@ = @Program.globSym@;
         @}
       ;

       //DefIdent überall sichtbar
Def: IDENT EQUAL Lambda @{
        @i @Lambda.sym@ = @Def.globSym@;
        @i @Def.globVal@ = @IDENT.val@;
	@i @Lambda.regList@ = newRegList();
        
        @t checkGlobalSymbol(@Def.globSym@, @IDENT.val@);
        
        @reg @Lambda.node@->name = @IDENT.val@;
        @reg @Lambda.node@->labelNum = -1;
        @reg @Lambda.node@->regList=@Lambda.regList@;
        
        @codegen @revorder(1) genSymbol(@IDENT.val@);
     @}
   ;

	//DefIdent in expr sichtbar
Lambda:	FUN IDENT ARROW Expr END @{
            @i @Expr.sym@ = addSymbol(@Lambda.sym@, @IDENT.val@);
	    @i @Lambda.node@ = newNode(OP_EVAL, @Expr.node@, NULL);
	    
            @t checkUnknownSymbol(@Lambda.sym@, @IDENT.val@);
            
            @reg assignIdentToReg(@Lambda.regList@, getParamReg(), @Expr.sym@);

	    @reg @Lambda.node@->regname = getResultReg();
            @reg @Expr.node@->regname = @Lambda.node@->regname;
            @reg @Expr.node@->regList = @Lambda.regList@;
            
	    @codegen genLabel(@Lambda.node@->name);
	    @codegen if(@Lambda.node@->labelNum >= 0) {markReg(@Lambda.regList@, @Lambda.node@->regname, REG_USED); restoreEnvironment(@Lambda.regList@, @Lambda.sym@);}
	    @codegen burm_label(@Lambda.node@); burm_reduce(@Lambda.node@, 1);
        @}
      ;

Expr: IF Expr THEN Expr ELSE Expr END @{
        @i @Expr.0.node@ = newNode(OP_IF, @Expr.1.node@, newNode(OP_CONDEVAL, @Expr.2.node@, @Expr.3.node@));
        
        @t @Expr.2.node@->parent=@Expr.0.node@;
        @t @Expr.3.node@->parent=@Expr.0.node@;
        
        @reg @Expr.0.node@->labelNum = nextIfLabelNum();
        
        @reg @Expr.1.node@->regname = @Expr.0.node@->regname;
        @reg @Expr.1.node@->regList = @Expr.0.regList@; 
        @reg @Expr.2.node@->regname = @Expr.0.node@->regname;
        @reg @Expr.2.node@->regList = @Expr.0.regList@;
        @reg @Expr.3.node@->regname = @Expr.0.node@->regname;
        @reg @Expr.3.node@->regList = @Expr.0.regList@;
      @} 
    | Lambda @{
	@i @Expr.node@ = newNode(OP_CLOSURE, @Lambda.node@, NULL);
	@i @Lambda.regList@ = newRegList();
	@i @Lambda.sym@ = copyList(@Expr.sym@);
	
	@reg @Lambda.node@->labelNum = nextIfLabelNum();
	@reg @Lambda.node@->name = labelNameFromNum("lambda",@Lambda.node@->labelNum);
	@reg @Expr.node@->ident = @Expr.sym@;
        @reg @Lambda.node@->regList=@Lambda.regList@;
    @}
    | LET IDENT EQUAL Expr IN Expr END @{ 
          @i @Expr.2.sym@ = addSymbol(@Expr.0.sym@, @IDENT.val@);
          @i @Expr.0.node@ = newNode(OP_LET, @Expr.1.node@, @Expr.2.node@);
          
          @t checkUnknownSymbol(@Expr.1.sym@, @IDENT.val@);
          @t checkUnknownSymbol(@Expr.0.sym@, @IDENT.val@); 
          
          @reg @Expr.1.node@->regname = getNextReg(@Expr.regList@, @Expr.0.node@->regname);
          @reg @Expr.1.node@->regList = @Expr.regList@;
          @reg assignIdentToReg(@Expr.regList@, @Expr.1.node@->regname, @Expr.2.sym@);
          @reg @Expr.2.node@->regname = @Expr.0.node@->regname;
          @reg @Expr.2.node@->regList = @Expr.0.regList@;
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
          @reg @Term.0.node@->regList = @Expr.0.regList@;
          @reg @Term.1.node@->regList = @Expr.0.regList@;
      @}
    | Term LESS Term @{
          @i @Expr.node@ = newNode(OP_LESS, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.regList@, @Expr.node@->regname);
          @reg @Term.0.node@->regList = @Expr.0.regList@;
          @reg @Term.1.node@->regList = @Expr.0.regList@;
      @}
    | Term EQUAL Term @{
          @i @Expr.node@ = newNode(OP_EQUAL, @Term.0.node@, @Term.1.node@);
          
          @reg @Term.0.node@->regname = @Expr.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@Expr.regList@, @Expr.node@->regname);
          @reg @Term.0.node@->regList = @Expr.0.regList@;
          @reg @Term.1.node@->regList = @Expr.0.regList@;
      @}
    | Expr Term @{
	  @i @Expr.0.node@ = newNode(OP_CALL, @Expr.1.node@,  @Term.node@);
	  
	  @reg @Term.node@->regname = getNextReg(@Expr.regList@,@Expr.0.node@->regname);
	  @reg @Expr.1.node@->regname = @Expr.node@->regname;
          @reg @Term.0.node@->regList = @Expr.0.regList@;
          @reg @Expr.1.node@->regList = @Expr.0.regList@;
          
	  @reg @Term.node@->ident = @Term.sym@;
      @}
    ;

EExpr: Term
     | KExpr
     ;

KExpr: NOT EExpr @{
          @i @KExpr.node@ = newNode(OP_NOT, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
          @reg @EExpr.node@->regList = @KExpr.regList@;
       @}
     | HEAD EExpr @{
          @i @KExpr.node@ = newNode(OP_HEAD, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
          @reg @EExpr.node@->regList = @KExpr.regList@;
       @}
     | TAIL EExpr @{
          @i @KExpr.node@ = newNode(OP_TAIL, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
          @reg @EExpr.node@->regList = @KExpr.regList@;
       @}
     | ISNUM EExpr @{
          @i @KExpr.node@ = newNode(OP_ISNUM, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
          @reg @EExpr.node@->regList = @KExpr.regList@;
       @}
     | ISLIST EExpr @{
          @i @KExpr.node@ = newNode(OP_ISLIST, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
          @reg @EExpr.node@->regList = @KExpr.regList@;
       @}
     | ISFUN EExpr @{
          @i @KExpr.node@ = newNode(OP_ISFUN, @EExpr.node@, NULL);
          
          @reg @EExpr.node@->regname = @KExpr.node@->regname;
          @reg @EExpr.node@->regList = @KExpr.regList@;
       @}
     ;

AddTerm: Term PLUS Term @{
            @i @AddTerm.node@ = newNode(OP_PLUS, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.1.node@->regname = getNextReg(@AddTerm.regList@, @AddTerm.node@->regname);
          @reg @Term.0.node@->regname = @AddTerm.node@->regname;
          @reg @Term.0.node@->regList = @AddTerm.0.regList@;
          @reg @Term.1.node@->regList = @AddTerm.0.regList@;
          
         @}
       | AddTerm PLUS Term @{
            @i @AddTerm.0.node@ = newNode(OP_PLUS, @AddTerm.1.node@, @Term.node@);
            
            @reg @AddTerm.1.node@->regname = @AddTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@AddTerm.regList@, @AddTerm.0.node@->regname);
          @reg @Term.0.node@->regList = @AddTerm.0.regList@;
          @reg @AddTerm.1.node@->regList = @AddTerm.0.regList@;
         @}
       ;

MultTerm: Term MULT Term @{
            @i @MultTerm.node@ = newNode(OP_MULT, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @MultTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@MultTerm.regList@, @MultTerm.node@->regname);
          @reg @Term.0.node@->regList = @MultTerm.0.regList@;
          @reg @Term.1.node@->regList = @MultTerm.0.regList@;
          @}
        | MultTerm MULT Term @{
            @i @MultTerm.0.node@ = newNode(OP_MULT, @MultTerm.1.node@, @Term.node@);
            
            @reg @MultTerm.1.node@->regname = @MultTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@MultTerm.regList@, @MultTerm.0.node@->regname);
          @reg @Term.0.node@->regList = @MultTerm.0.regList@;
          @reg @MultTerm.1.node@->regList = @MultTerm.0.regList@;
          @}
        ;

AndTerm: Term AND Term @{
            @i @AndTerm.node@ = newNode(OP_AND, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @AndTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@AndTerm.regList@, @AndTerm.node@->regname);
          @reg @Term.0.node@->regList = @AndTerm.0.regList@;
          @reg @Term.1.node@->regList = @AndTerm.0.regList@;
         @}
       | AndTerm AND Term @{
            @i @AndTerm.0.node@ = newNode(OP_AND, @AndTerm.1.node@, @Term.node@);
            
            @reg @AndTerm.1.node@->regname = @AndTerm.0.node@->regname;
            @reg @Term.0.node@->regname = getNextReg(@AndTerm.regList@, @AndTerm.0.node@->regname);
          @reg @Term.0.node@->regList = @AndTerm.0.regList@;
          @reg @AndTerm.1.node@->regList = @AndTerm.0.regList@;
          @}
       ;

DotTerm: Term DOT Term @{
            @i @DotTerm.node@ = newNode(OP_DOT, @Term.0.node@, @Term.1.node@);
            
          @reg @Term.0.node@->regname = @DotTerm.node@->regname;
          @reg @Term.1.node@->regname = getNextReg(@DotTerm.regList@, @DotTerm.node@->regname);
          @reg @Term.0.node@->regList = @DotTerm.0.regList@;
          @reg @Term.1.node@->regList = @DotTerm.0.regList@;
         @}
       | Term DOT DotTerm @{
            @i @DotTerm.0.node@ = newNode(OP_DOT, @Term.node@, @DotTerm.1.node@);
            
            @reg @DotTerm.1.node@->regname = getNextReg(@DotTerm.regList@, @DotTerm.0.node@->regname);
            @reg @Term.0.node@->regname = (@DotTerm.0.node@->regname);
          @reg @Term.0.node@->regList = @DotTerm.0.regList@;
          @reg @DotTerm.1.node@->regList = @DotTerm.0.regList@;
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
        @reg @Term.node@->regList = @Term.regList@;
      @}
    ;

