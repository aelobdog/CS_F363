#include "../lexer/lexer.h"
#include "parser.h"
#include "parserDef.h"
#include <stdio.h>
// #include "lexerDef.h"

void printRule(prodRule* r) {
    printf("%s ->", getStringOf(r->nonTerminal));
    for (int i = 0; i < r->termsInExpansion -1; i++) {
        printf(" %s", getStringOf(r->expansion[i]));
    }
    printf("\n");
}

int main() {
    // hash table stuff for the symbol table
   printf("Printing rule set :\n");
    gram g = readGram();
    for(int i=0;i<g.numberOfRules;i++) {
        printRule(&(g.rules[i]));
    }
    return 0;
}
