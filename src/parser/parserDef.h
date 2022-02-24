#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "../lexer/lexerDef.h"
#include "../lexer/lexer.h"

typedef struct {
    termType nonTerminal;
    termType expansion[15];
    int termsInExpansion;
} prodRule;

typedef struct {
    token first[15];
    token follow[15];
} firstAndFollow;

typedef struct gram {
    prodRule rules[100];
    int numberOfRules;
} gram;

#endif