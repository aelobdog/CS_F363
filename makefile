driver:
	gcc -o bin/driver driver.c src/lexer/lexer.c -ggdb

parser:
	gcc -o bin/parser src/lexer/lexer.c src/parser/parser.c -ggdb

runp:
	reset; make parser; ./bin/parser
rund:
	reset; make driver; ./bin/driver