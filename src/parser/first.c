#include <stdio.h>
#include "parserDef.h"
#include "../lexer/lexer.h"

// #define GRAMMARFILE "../../grammar.txt"

gram readGram() {
    gram g;
    int rulenum = 0;
    int termnum = 0;
    char buf[100];
    char c;
    FILE* f = fopen("src/parser/grammar.txt", "r");
    if (!f) perror("GRAMMAR FILE ERROR :");

    while (fscanf(f, "%s%c", buf, &c) == 2) {
        // printf("%s\n",buf);
        if (getTypeOf(buf) != TK_EOF) {
            if (termnum) {
                g.rules[rulenum].expansion[termnum-1] = getTypeOf(buf);
            } else {
                g.rules[rulenum].nonTerminal= getTypeOf(buf);
            }
            termnum++;
        }
        if (c == '\n') {
            g.rules[rulenum].termsInExpansion = termnum;
            termnum = 0;
            rulenum++;
        }
    }
    g.rules[rulenum].expansion[termnum-1] = getTypeOf(buf);
    //printf("%s\n",buf);

    g.numberOfRules = rulenum+1;
    fclose(f);
    return g;
}

int first(token nt, token* exp, firstAndFollow ff, int num) {
    int a = 0, b = 0, eps = 0;
    
    

}