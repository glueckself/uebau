all: ag 

clean:
	rm -f ag lex.yy.c y.tab.* oxout.*

oxout.y oxout.l: parser.y scanner.l symbols.h
	ox parser.y scanner.l

y.tab.c: oxout.y
	bison -yd oxout.y

lex.yy.c: oxout.l
	flex oxout.l

ag: lex.yy.c y.tab.c symbols.c symbols.h
	gcc -o ag lex.yy.c y.tab.c symbols.c -lfl

debug: lex.yy.c y.tab.c symbols.c symbols.h
	gcc -DDEBUG -g -o ag lex.yy.c y.tab.c symbols.c -lfl
