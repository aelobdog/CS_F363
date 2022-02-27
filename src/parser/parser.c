#include "../lexer/lexer.h"
#include "parser.h"
#include "parserDef.h"
#include <stdio.h>
#include <string.h>

// #include "lexerDef.h"

// TODO: memset grammar too ?

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
   FILE* f = fopen("src/parser/fullgram.txt", "r");
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
         g.rules[rulenum].termsInExpansion = termnum - 1;
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
   ff->sets[0].numFollow = 1;
   ff->sets[0].hasDollar = 1;
   for (k = PROGRAM+1; k < TERMTYPESIZE - 1; k++) {
      // printf("< ----- computing follow of %s\n", getStringOf(k));
      follow(k, ff, k-PROGRAM, g);
   }

   // TODO: brute force to add first(morefields) and follow(fieldDefinitions) to follow(fielddefinition)
   firstAndFollow* fd = &(ff->sets[FIELDDEFINITION - PROGRAM]);
   firstAndFollow* mf = &(ff->sets[MOREFIELDS - PROGRAM]);
   firstAndFollow* fds = &(ff->sets[FIELDDEFINITIONS - PROGRAM]);

   for (int i = 0; i < mf->numFirst; i++) {
      int add = 1;
      for (int j=0; j < fd->numFollow; j++) {
         if(mf->first[i] == fd->follow[j] || mf->first[i] == EPS) { add = 0; break; } 
      }
      if (add) fd->follow[fd->numFollow++] = mf->first[i]; // not performing dollar inclusion
   }

   for (int i = 0; i < fds->numFollow; i++) {
      int add = 1;
      for (int j=0; j < fd->numFollow; j++) {
         if(fds->follow[i] == fd->follow[j]) { add = 0; break; } 
      }
      if (add) fd->follow[fd->numFollow++] = fds->follow[i]; // not performing dollar inclusion
   }
}

void follow(termType nt, ffSets* fS, int index, gram *g) {
   // get a handle to the firstAndFollow structure corresponding to 'nt'
   firstAndFollow *handleB = &(fS->sets[index]);
   
   // to find the follow set, we must iterate through every rule in the grammar,
   // looking for rules that conform to the required form A -> aBb where
   // both a, b can be epsilon, B is 'nt'
   for (int i = 0; i < g->numberOfRules; i++) {
      int j = 0;
      // in every rule, we must look for the non-terminal 'nt'.
      // we skip all the terms in the expansion that are not 'nt'.
      // at the end of this loop, j is the index of 'nt' in the expansion.
      for (j = 0; j < g->rules[i].termsInExpansion && g->rules[i].expansion[j] != nt; j++);
      j++; // now j is the index of the first term to the right of 'nt'.
      
      // if (nt == MAINFUNCTION) {
      //    printf("\n-----------\nin rule (expansions = %d) \n", g->rules[i].termsInExpansion);
      //    printRule(&g->rules[i]);
      //    printf("j is %d for nt ( %s )\n", j, getStringOf(nt));
      // }
      
      // at this point, there are only two possibilities. Either 'b' is epsilon, or it is not
      if (j == g->rules[i].termsInExpansion) {
         // printf("in here for %s\n", getStringOf(g->rules[i].nonTerminal));
         // for rules of the form A -> aB we must add all the elements of the follow set of A
         // to the follow set of B. To do so, we first get a handle to the first and follow
         // sets of A.
         firstAndFollow* handleA = &(fS->sets[ g->rules[i].nonTerminal - PROGRAM ]);
         // now we can iterate over all the elements of the follow set of A
         for (int r = 0; r < handleA->numFollow; r++) {
            // we need to also make sure not to add duplicates to the follow of B so we iterate
            // through B's follow to make sure that the element we with to add is not in it already.
            int add = 1;
            for (int t = 0; t < handleB->numFollow; t++) {
               if (handleA->follow[r] == handleB->follow[t]) { add = 0; break; }
            }
            if (add) {
               handleB->follow[handleB->numFollow ++] = handleA->follow[r];
               if (handleA->follow[r] == DOLLAR) handleB->hasDollar = 1;
            }
         }
      } else {
         // if 'b' is not epsilon, that means we must check the first sets of all the elements in
         // in 'b', stopping only if we encounter
         //    1) a terminal
         //    2) a non terminal whose first set does not have epsilon
         for (int r = j; r < g->rules[i].termsInExpansion; r++) {
            
            // if (nt == INPUT_PAR) {
            //    printf("%s is %s\n", getStringOf(g->rules[i].expansion[r]), (g->rules[i].expansion[r] < 58) ? "terminal" : "non terminal");
            // }

            // if the term in the expansion is a terminal, we simply add it to the follow set and
            // stop processing the current rule.
            if (g->rules[i].expansion[r] < PROGRAM) {
               int add = 1;
               for (int s = 0; s < handleB->numFollow; s++) {
                  if (handleB->follow[s] == g->rules[i].expansion[r]) { add = 0; break; }
               }
               if (add) handleB->follow[handleB->numFollow++] = g->rules[i].expansion[r];
               break;
            }

            // we then get a handle to the first set of the term under consideration
            firstAndFollow* handleT = &(fS->sets[ g->rules[i].expansion[r] - PROGRAM ]);
            // we can now iterate over all its elements and add them to B's follow set if they are
            // not already present in it.
            for (int k = 0; k < handleT->numFirst; k++) {
               int add = 1;
               for (int m = 0; m < handleB->numFollow; m++) {
                  if (handleT->first[k] == handleB->follow[m] || handleT->first[k] == EPS) { add = 0; break; }
               }
               if (add) handleB->follow[handleB->numFollow++] = handleT->first[k];
            }
            // now we can check if the term we just precocesses has epsilon in its first set
            // if it doesn't have epsilon, we can stop processing the current rule.
            if (! handleT->hasEps) break;

            // also, if we reach the last rule, and the last rule has epsilon in its first set,
            // we must add all elemets of follow of A to follow of B
            if (r == g->rules[i].termsInExpansion - 1) {
               firstAndFollow* handleA = &(fS->sets[ g->rules[i].nonTerminal - PROGRAM ]);
               // now we can iterate over all the elements of the follow set of A
               for (int d = 0; d < handleA->numFollow; d++) {
                  // we need to also make sure not to add duplicates to the follow of B so we iterate
                  // through B's follow to make sure that the element we with to add is not in it already.
                  int add = 1;
                  for (int t = 0; t < handleB->numFollow; t++) {
                     if (handleA->follow[d] == handleB->follow[t]) { add = 0; break; }
                  }
                  if (add) {
                     handleB->follow[handleB->numFollow ++] = handleA->follow[d];
                     if (handleA->follow[r] == DOLLAR) handleB->hasDollar = 1;
                  }
               }
            }
         }
      }
   }
}

int first(termType nt, ffSets* fS, int index, gram* g) {
   firstAndFollow* ff = &(fS->sets[index]);
   for (int i = 0; i < g->numberOfRules; i++) {

      if (g->rules[i].nonTerminal != nt) continue; // skip the rule if it is not for the non-terminal we want first for

      if (g->rules[i].expansion[0] < PROGRAM) { // PROGRAM is the enum value of the first non-terminal (ref. src/lexer/lexerDef.h)
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
            if (g->rules[i].expansion[j] >= PROGRAM) {
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

// assumes that first and follow sets have been computed
void populateParseTable(parseTable* pTable, gram* g, ffSets* fS) {
   for(int i = 0; i < g->numberOfRules; i++) { // iterate over all the rules
      for(int j = 0; j < g->rules[i].termsInExpansion; j++) { // iterate over all the terms in the expansion of the rule selected
         if(g->rules[i].expansion[j] < PROGRAM) { // if the current term on the rhs is a terminal, add a rule for it directly in the pTable
            // printf("in rule of ( %s ) rhs symbol ( %s )\n", getStringOf(g->rules[i].nonTerminal), getStringOf(g->rules[i].expansion[j]));
            pTable->entry [g->rules[i].nonTerminal - PROGRAM][g->rules[i].expansion[j]] = g->rules[i];
            pTable->hasRule [g->rules[i].nonTerminal - PROGRAM][g->rules[i].expansion[j]] = 1;
            // printf("%d, %d, %d\n", g->rules[i].nonTerminal - PROGRAM, g->rules[i].expansion[j], pTable->hasRule [g->rules[i].nonTerminal - PROGRAM][g->rules[i].expansion[j]]);
            break; // break outof term loop, go to next next rule
         }
         
         else { // if current term on the rhs is a non-terminal, we look at its first and process further
            if (g->rules[i].expansion[j] == EPS) {
               firstAndFollow* handleA = &(fS->sets[g->rules[i].nonTerminal - PROGRAM]);
               for(int m = 0; m < handleA->numFollow; m++) {
                  if (handleA->follow[m] != DOLLAR) {
                     pTable->entry [g->rules[i].nonTerminal - PROGRAM][handleA->follow[m]] = g->rules[i];
                     pTable->hasRule [g->rules[i].nonTerminal - PROGRAM][handleA->follow[m]] = 1;
                  }
               }
               if (fS->sets[g->rules[i].nonTerminal - PROGRAM].hasDollar) {
                  // second arg is PROGRAM because we want to set the entry for '$' whose index is assumes to be 1 more than the index for the last terminal
                  pTable->entry[g->rules[i].nonTerminal - PROGRAM][PROGRAM] = g->rules[i];
                  pTable->hasRule[g->rules[i].nonTerminal - PROGRAM][PROGRAM] = 1;
               }
            }
            
            firstAndFollow* handleT = &(fS->sets[g->rules[i].expansion[j] - PROGRAM]); // get a handle to the first and follow sets of the non terminal
            for (int k = 0; k < handleT->numFirst; k++) { // adding the rule to all the terminal entries in the first set
               if (handleT->first[k] != EPS) {
                  pTable->entry [g->rules[i].nonTerminal - PROGRAM][handleT->first[k]] = g->rules[i];
                  pTable->hasRule [g->rules[i].nonTerminal - PROGRAM][handleT->first[k]] = 1;
               }
            }
            if (! handleT->hasEps) break; // break outof term loop, go to next next rule

            if (j == g->rules[i].termsInExpansion - 1) { // on the last term, and it has eps in its first
               firstAndFollow* handleA = &(fS->sets[g->rules[i].nonTerminal - PROGRAM]);
               for(int m = 0; m < handleA->numFollow; m++) {
                  if (handleT->follow[m] != DOLLAR) {
                     pTable->entry [g->rules[i].nonTerminal - PROGRAM][handleT->follow[m]] = g->rules[i];
                     pTable->hasRule [g->rules[i].nonTerminal - PROGRAM][handleT->follow[m]] = 1;
                  }
               }
               
               if (fS->sets[g->rules[i].nonTerminal - PROGRAM].hasDollar) {
                  // second arg is PROGRAM because we want to set the entry for '$' whose index is assumes to be 1 more than the index for the last terminal
                  pTable->entry[g->rules[i].nonTerminal - PROGRAM][PROGRAM] = g->rules[i];
                  pTable->hasRule[g->rules[i].nonTerminal - PROGRAM][PROGRAM] = 1;
               }
            }
         }
      }
    }

   // printf("printing table :\n    ");
   //  for (int j = 0; j < PROGRAM+1; j++) printf("%2d ", j);
   //  printf("\n");


   //  for (int i = 0; i < TERMTYPESIZE - PROGRAM - 1; i++) {
   //    printf("%2d> ", i+1);
   //     for (int j = 0; j < PROGRAM+1; j++) {
   //        printf("%2d ", pTable->hasRule[i][j]);
   //     }
   //     printf("\n");
   //  }
}

termType getStackTop(parseStack* pStack) {
   // maybe do bounds checking?
   return pStack->stack[pStack->top];
}

termType popStackTop(parseStack* pStack) {
   pStack->top -= 1;
   return pStack->stack[pStack->top + 1];
}

void pushStackTop(parseStack* pStack, termType term, parseTable* pTable, tokenList* tList) {
   prodRule* rule = &pTable->entry[term - PROGRAM][tList->list[tList->current].type]; // get rule from parse table
   pStack->top ++;
   for (int i = rule->termsInExpansion - 1; i >= 0; i--) {
      pStack->stack[pStack->top++] = rule->expansion[i];
   }
   pStack->top --;
}

void initStack(parseStack* pStack) {
   pStack->top = 0;
   pStack->stack[pStack->top++] = DOLLAR;
   pStack->stack[pStack->top] = PROGRAM;
}

int main() {
   ffSets ff;
   parseTable pTable;
   parseStack pStack;
   tokenList tList;
   twinBuffer b;
   FILE* source;
   int eof;
   hashTableEntry globalHashTable[HASHTABLESIZE];

   memset(&pTable, 0, sizeof(pTable));
   memset(&ff, 0, sizeof(ff));
   memset(&pStack, 0, sizeof(pStack));
   memset(&tList, 0, sizeof(tList));
   
   gram g = readGram();
   computeFirsts(&g, &ff);
   computeFollows(&g, &ff);
   populateParseTable(&pTable, &g, &ff);
   initStack(&pStack);

   initLexerDefaults("test.txt", source, &b, &eof, globalHashTable, &tList);

   printf("\n\n\n");

   for (int i=0; i<=pStack.top; i++) printf("%10s  ", getStringOf(pStack.stack[i]));
   printf("\n");

   termType t;

   t = popStackTop(&pStack);
   for (int i=0; i<=pStack.top; i++) printf("%10s  ", getStringOf(pStack.stack[i]));
   printf("\n");

   pushStackTop(&pStack, t, &pTable, &tList);
   for (int i=0; i<=pStack.top; i++) printf("%10s  ", getStringOf(pStack.stack[i]));
   printf("\n");

   return 0;
}