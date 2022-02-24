driver:
	gcc -o bin/driver driver.c src/lexer/lexer.c -ggdb

parser:
	gcc -o bin/parser src/parser/parser.c src/parser/first.c src/lexer/lexer.c -ggdb