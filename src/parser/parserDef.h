#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "../lexer/lexerDef.h"
#include "../lexer/lexer.h"

#define MAX_DISTINCT_NONTERMS 100

typedef struct {
    termType nonTerminal;
    termType first[15];
    termType follow[15];
    int numFirst;
    int numFollow;
    int hasEps;
    int computedFirst;
} firstAndFollow;

typedef struct {
    termType nonTerminal;
    termType expansion[15];
    int termsInExpansion;
} prodRule;

typedef struct gram {
    prodRule rules[MAX_DISTINCT_NONTERMS];
    int numberOfRules;
} gram;

typedef struct ffSets {
    firstAndFollow sets[MAX_DISTINCT_NONTERMS];
    int numberOfFFS;
} ffSets;

#endif