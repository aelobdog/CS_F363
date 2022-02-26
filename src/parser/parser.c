#include "../lexer/lexer.h"
#include "parser.h"
#include "parserDef.h"
#include <stdio.h>
#include <string.h>

// #include "lexerDef.h"

void printRule(prodRule* r) {
    printf("%s ->", getStringOf(r->nonTerminal));
    for (int i = 0; i < r->termsInExpansion -1; i++) {
        printf(" %s", getStringOf(r->expansion[i]));
    }
    printf("\n");
}

gram readGram() {
    gram g;
    int rulenum = 0;
    int termnum = 0;
    char buf[100];
    char c;
    FILE* f = fopen("src/parser/grammar.txt", "r");
    if (!f) perror("GRAMMAR FILE ERROR :");

    while (fscanf(f, "%s", buf) == 1) {
       c = fgetc(f);
       if (termnum) {
          g.rules[rulenum].expansion[termnum-1] = getTypeOf(buf);
       } else {
          g.rules[rulenum].nonTerminal= getTypeOf(buf);
       }
       termnum++;
       if (c == '\n') {
          g.rules[rulenum].termsInExpansion = termnum;
          termnum = 0;
          rulenum++;
        }
    }
    g.rules[rulenum].expansion[termnum-1] = getTypeOf(buf);
    g.numberOfRules = rulenum + 1;
    fclose(f);
    return g;
}

void computeFirsts(gram *g, ffSets *ff) {
    int k;
    for (k = PROGRAM; k < TERMTYPESIZE - 1; k++) {
        ff->sets[k-PROGRAM].nonTerminal = k;
        first(k, ff, k-PROGRAM, g);
    }
    ff->numberOfFFS = k - PROGRAM;
}

// call after computeFirsts !
// assumes that computeFirsts sets the values of '.nonTerminal' and 'numberOfFFS' fields
void computeFollows(gram *g, ffSets *ff) {
    int k;
    ff->sets[0].follow[0] = DOLLAR; // hardcoding the follow if start symbol "program" to $
    ff->sets[0].numFollow++;
    for (k = PROGRAM+1; k < TERMTYPESIZE - 1; k++) {
        follow(k, ff, k-PROGRAM, g);
    }
}

int follow(termType nt, ffSets* fS, int index, gram *g) {
    firstAndFollow* ff = &(fS->sets[index]); // the set that is to be filled with the follow terms
    int gotTerminal = 0;
    
    for (int i = 0; i < g->numberOfRules; i++) { // iterates over all the rules
        gotTerminal = 0;
        for (int j = 0; j < g->rules[i].termsInExpansion; j++) {
            if (nt == g->rules[i].expansion[j]) { // found the non terminal on the rhs of the rule
                int k = 0;
                for (k = j + 1; k < g->rules[i].termsInExpansion; k++) { // iterate through the terms in the expansion to the right of the non-terminal
                    
                    if (g->rules[i].expansion[k] < 58) { // found terminal, add it to follow set if it doesn't already exist in the follow
                        int add = 1;
                        for (int p = 0; p < ff->numFollow; p++) {
                            if (ff->follow[p] == g->rules[i].expansion[k]) { add = 0; break; }
                        }
                        if (add) ff->follow[ff->numFollow++] = g->rules[i].expansion[k];
                        gotTerminal = 1;
                        break;
                    }
                    else { // found non-terminal
                        firstAndFollow* other = &fS->sets[g->rules[i].expansion[k] - PROGRAM];
                        for (int q = 0; q < other->numFirst; q++) { // iterate over the non terminal's first set
                            int add = 1;
                            for (int m = 0; m < ff->numFollow; m++) { // iterate over all the elements in the follow set of the current non terminal
                                if (ff->follow[m] == other->first[q] || other->first[q] == EPS /*||  other->first[q] == TK_EOF check! */) { add = 0; break; } // check if term in first already exists in follow, and add it if not
                            }
                            if (add && other->first[q] != EPS) ff->follow[ff->numFollow++] = other->first[q];
                        }
                        if (! other->hasEps) break; // if the current non terminal's first set does not have a epsilon, then stop follow calculation for that rule
                    }

                } // k reached the end of expansion

                if (! gotTerminal) {
                    k = k - 1;
                    if (g->rules[i].nonTerminal != g->rules[i].expansion[k]) { // if the non terminal whose follow we wish to calculate appears as the last term in a rule's expansion, we skip this part
                        firstAndFollow* lhs = &fS->sets[g->rules[i].nonTerminal - PROGRAM];
                        if ((k == g->rules[i].termsInExpansion - 1 && nt == g->rules[i].expansion[k]) || fS->sets[g->rules[i].expansion[k] - PROGRAM].hasEps) {
                            for (int q = 0; q < lhs->numFollow; q++) { // iterate over the LHS non terminal's follow set
                                int add = 1;
                                printf("%s >>> %s\n", getStringOf(lhs->nonTerminal), getStringOf(lhs->follow[q]));
                                for (int m = 0; m < ff->numFollow; m++) { // iterate over all the elements in the follow set of the current non terminal
                                    if (ff->follow[m] == lhs->follow[q]) { add = 0; break; } // check if term in first already exists in follow, and add it if not
                                }
                                if (add) ff->follow[ff->numFollow++] = lhs->follow[q];
                                printf("%s > %s", getStringOf(ff->nonTerminal), getStringOf(ff->follow[ff->numFollow-1]));
                            }
                        }
                    }
                }
            }
        }
    }
}

// return value = hasEps
int first(termType nt, ffSets* fS, int index, gram *g) {
    firstAndFollow* ff = &(fS->sets[index]);
    for (int i = 0; i < g->numberOfRules; i++) {

        if (g->rules[i].nonTerminal != nt) continue; // skip the rule if it is not for the non-terminal we want first for

        if (g->rules[i].expansion[0] < 58) { // 58 is the enum value of the first non-terminal (ref. src/lexer/lexerDef.h)
            int add = 1;
            for (int k = 0; k < ff->numFirst; k++) {
                if (ff->first[k] == g->rules[i].expansion[0]) { add = 0; break; }
            }
            if (add) ff->first[ff->numFirst++] = g->rules[i].expansion[0];
        } 
        
        else if (g->rules[i].expansion[0] == EPS) {
            ff->hasEps = 1;

            int add = 1;
            for (int k = 0; k < ff->numFirst; k++) {
                if (ff->first[k] == EPS) { add = 0; break; }
            }
            if (add) ff->first[ff->numFirst++] = EPS;
        } 
        
        else {
            // int terminalReached = 0;
            firstAndFollow* other;
            int localHasEps = 2;
            for (int j = 0; j < g->rules[i].termsInExpansion; j++) {

                // if it is a non terminal
                if (g->rules[i].expansion[j] >= 58) {
                    if (g->rules[i].expansion[j] != nt) {
                        if (! fS->sets[g->rules[i].expansion[j] - PROGRAM].computedFirst) {
                            localHasEps = first(g->rules[i].expansion[j], fS, g->rules[i].expansion[j] - PROGRAM, g);
                        } else {
                            localHasEps = fS->sets[g->rules[i].expansion[j] - PROGRAM].hasEps;
                        }
                    }

                    if (g->rules[i].expansion[j] == nt) {
                        if (fS->sets[g->rules[i].expansion[j-1] - PROGRAM].hasEps) {
                            ff->hasEps = 1;
                            int add = 1;
                            for (int k = 0; k < ff->numFirst; k++) {
                                if (ff->first[k] == g->rules[i].expansion[j]) { add = 0; break; }
                            }
                            if (add) ff->first[ff->numFirst++] = g->rules[i].expansion[j];
                        }
                        continue;
                    }

                    other = &fS->sets[g->rules[i].expansion[j] - PROGRAM];
                    for (int q = 0; q < other->numFirst; q++) {
                        int add = 1;
                        for (int m = 0; m < ff->numFirst; m++) {
                            if (ff->first[m] == other->first[q] || other->first[q] == EPS /*|| other->first[q] == TK_EOF*/) { add = 0; break; }
                        }
                        if (add) ff->first[ff->numFirst++] = other->first[q];
                    }

                    if (localHasEps==0) break;
                }

                // if it is a terminal
                else {
                    localHasEps = 0;
                    int add = 1;
                    for (int k = 0; k < ff->numFirst; k++) {
                        if (ff->first[k] == g->rules[i].expansion[j]) { add = 0; break; }
                    }
                    if (add) ff->first[ff->numFirst++] = g->rules[i].expansion[j];
                    break;
                }
            }

            if (localHasEps == 1) {
                ff->hasEps = 1;
                int add = 1;
                for (int k = 0; k < ff->numFirst; k++) {
                    if (ff->first[k] == EPS) { add = 0; break; }
                }
                if (add) ff->first[ff->numFirst++] = EPS;
            }

        }
    }
    ff->computedFirst = 1;
    return ff->hasEps;
}

int main() {
    // hash table stuff for the symbol table
    gram g = readGram();
    // for(int i=0;i<g.numberOfRules;i++) {
    //     printRule(&(g.rules[i]));
    // }
    ffSets ff;
    memset(&ff, 0, sizeof(ffSets));
    computeFirsts(&g, &ff);
    computeFollows(&g, &ff);

    printf("first and follow sets\n");
    for(int i=0;i<ff.numberOfFFS; i++) {
        if (0 == ff.sets[i].numFirst) continue;

        printf("%s =", getStringOf(ff.sets[i].nonTerminal));
        for (int j=0; j<ff.sets[i].numFirst; j++) {
            printf(" %s", getStringOf(ff.sets[i].first[j]));
        }
        printf("\n");
    }

    printf("\n\n-------------------------------\n\n");

    for(int i=0;i<ff.numberOfFFS; i++) {
        if (0 == ff.sets[i].numFollow) continue;

        printf("%s =", getStringOf(ff.sets[i].nonTerminal));
        for (int j=0; j<ff.sets[i].numFollow; j++) {
            printf(" %s", getStringOf(ff.sets[i].follow[j]));
        }
        printf("\n");
    }
    return 0;
}
