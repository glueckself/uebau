all: parser

clean:
	rm -f parser lex.yy.c y.tab.*

y.tab.c: parser.y
	bison -yd parser.y

lex.yy.c: scanner.l
	flex scanner.l

parser: lex.yy.c y.tab.c
	gcc -o parser lex.yy.c y.tab.c -lfl
