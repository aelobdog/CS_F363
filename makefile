driver: lexer.h parser.h lexerDef.h parserDef.h lexer.c parser.c driver.c
	gcc -o stage1exe lexer.c parser.c driver.c -ggdb

driver7: lexer.h parser.h lexerDef.h parserDef.h lexer.c parser.c driver.c
	gcc-7 -o stage1exe lexer.c parser.c driver.c -ggdb

# parser:
# 	gcc -o bin/parser src/lexer/lexer.c src/parser/parser.c parsertest.c -ggdb

# ptree:
# 	gcc -o bin/ptree src/lexer/lexer.c src/parser/parser.c parsetree.c -ggdb