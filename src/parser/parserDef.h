#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "../lexer/lexerDef.h"
#include "../lexer/lexer.h"

#define MAX_DISTINCT_NONTERMS 100
#define MAX_STACK_SIZE 2500

typedef struct {
    termType nonTerminal;
    termType first[15];
    termType follow[15];
    int numFirst;
    int numFollow;
    int hasEps;
    int hasDollar;
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

typedef struct parseTable {
    // TERMTYPESIZE - PROGRAM - 1 => number of non-terminals in the language spec
    // 57 = number of terminals in the language spec + 1 (for dollar)
    prodRule entry[TERMTYPESIZE - PROGRAM - 1][PROGRAM + 1];
    int hasRule[TERMTYPESIZE - PROGRAM - 1][PROGRAM + 1]; // using 'char' instead of 'int' to save memory
} parseTable;

typedef struct parseStack {
    termType stack[MAX_STACK_SIZE];
    int top;
} parseStack;

#endif