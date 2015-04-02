%{
/* need this for the call to atof() below */
#include <stdio.h>
#include <stdlib.h>
%}

DIGITS	[0-9]*
HEX	\$[0-9A-Fa-f]*
ID	[a-zA-Z][a-zA-Z0-9]*

%%
and|end|isnum|islist|isfun|fun|if|then|else|let|in|not|head|tail|";"|"="|"+"|"-"|"*"|"."|"<"|"="|"("|")"|"->" {
    printf("%s\n", yytext);
}

{ID} {
    printf("ident %s\n", yytext);
}

{DIGITS} {
    printf("num %u\n", strtoul(yytext, NULL, 0));
}

{HEX} {
    printf("num %u\n", strtoul(&(yytext[1]), NULL, 16));
}
    

[ \t\n]+        //eat whitespaces

"//".*
%%

int main(int argc, char *argv[]) {
	argv++;
        argc--;
        
	if(argc > 0)
		yyin=fopen(argv[0],"r");
	else
		yyin=stdin;

	return yylex();
}
