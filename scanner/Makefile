all: scanner

clean:
	rm -f scanner lex.yy.c

lex.yy.c: scanner.flex
	flex scanner.flex

scanner: lex.yy.c
	gcc -o scanner lex.yy.c -lfl
