all: codea

codea: asmgen.c parser_iburg.c y.tab.c lex.yy.c symbols.c
	gcc $(DEF) -o codea $^ -lfl

oxout.y oxout.l: parser.y scanner.l symbols.h
	ox parser.y scanner.l

y.tab.c y.tab.h: oxout.y
	bison -yd oxout.y

lex.yy.c: oxout.l
	flex oxout.l

parser_iburg.c: asmgen.bfe asmgen.h
	bfe asmgen.bfe | iburg > parser_iburg.c

clean:
	rm -f parser_iburg.c lex.yy.c y.tab.c oxout.y oxout.l codea y.tab.h

