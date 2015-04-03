all: parser

clean:
	rm -f parser lex.yy.c y.tab.* oxout.*

oxout.y oxout.l: parser.y scanner.l
	ox parser.y scanner.l

y.tab.c: oxout.y
	bison -yd oxout.y

lex.yy.c: oxout.l
	flex oxout.l

parser: lex.yy.c y.tab.c
	gcc -o parser lex.yy.c y.tab.c -lfl
