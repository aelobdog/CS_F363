driver:
	gcc -o bin/driver driver.c src/lexer/lexer.c -ggdb

parser:
	gcc -o bin/parser src/lexer/lexer.c src/parser/parser.c parsertest.c -ggdb

ptree:
	gcc -o bin/ptree src/lexer/lexer.c src/parser/parser.c parsetree.c -ggdb