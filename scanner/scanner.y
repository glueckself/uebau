%{
/* need this for the call to atof() below */
#include <math.h>

%}

DIGITS	[0-9]*
HEX	\$[0-9A-Fa-f]*
ID	[a-zA-Z][a-zA-Z0-9]*

%%

%%

int main(int argc, char *argv[]) {
	argv++, argv--;
	if(argc > 0)
		yyin=fopen(argv[0],"r");
	else
		yyin=stdin;

	return yylex();
}
