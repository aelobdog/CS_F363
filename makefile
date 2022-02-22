all:
	gcc -o bin/driver src/parser.c src/lexer.c -ggdb
