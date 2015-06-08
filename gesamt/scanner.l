%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "symbols.h"

#ifndef CODE
#define CODE
#endif
#include "asmgen.h"
%}

DIGITS	[0-9]*
HEX	\$[0-9A-Fa-f]*
ID	[a-zA-Z][a-zA-Z0-9]*

%%
and     return AND;
end     return END;
isnum   return ISNUM;
islist  return ISLIST;
isfun   return ISFUN;
fun     return FUN;
if      return IF;
then    return THEN;
else    return ELSE;
let     return LET;
in      return IN;
not     return NOT;
head    return HEAD;
tail    return TAIL;
";"     return SEMICOLON;
"="     return EQUAL;
"+"     return PLUS;
"-"     return MINUS;
"*"     return MULT;
"."     return DOT;
"<"     return LESS;
"("     return LBRACKET;
")"     return RBRACKET;
"->"    return ARROW;

{ID} {
	return IDENT; @{ @IDENT.val@ = strdup(yytext); @}
}

{DIGITS} {
	return NUM; @{ @NUM.val@ = strtoul(yytext, NULL, 0); @}
}

{HEX} {
	return NUM; @{ @NUM.val@ = strtoul(&(yytext[1]), NULL, 16); @}
}
    

[ \t\n]+        //eat whitespaces
"//".*		//eat comments

.		exit(1);
%%

int main(int argc, char *argv[]) {
	argv++;
        argc--;
        
	if(argc > 0)
		yyin=fopen(argv[0],"r");
	else
		yyin=stdin;

	if(yyparse())
		return 2; // 1 is handled by exit above
}



void yyerror (char const *s) {
  fprintf (stderr, "%s\n", s);
}

