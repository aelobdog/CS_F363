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
      if (add) fd->follow[fd->numFollow++] = mf->first[i];
   }

   for (int i = 0; i < fds->numFollow; i++) {
      int add = 1;
      for (int j=0; j < fd->numFollow; j++) {
         if(fds->follow[i] == fd->follow[j]) { add = 0; break; } 
      }
      if (add) fd->follow[fd->numFollow++] = fds->follow[i];
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
            if (add) handleB->follow[handleB->numFollow ++] = handleA->follow[r];
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
            if (g->rules[i].expansion[r] < 58) {
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
                  if (add) handleB->follow[handleB->numFollow ++] = handleA->follow[d];
               }
            }
         }
      }
   }
}
//    firstAndFollow* ff = &(fS->sets[index]); // the set that is to be filled with the follow terms
//    int gotTerminal = 0;
// 
//    for (int i = 0; i < g->numberOfRules; i++) { // iterates over all the rules
//       gotTerminal = 0;
//       for (int j = 0; j < g->rules[i].termsInExpansion; j++) {
//          if (nt == g->rules[i].expansion[j]) { // found the non terminal on the rhs of the rule
//             int k = 0;
//             for (k = j + 1; k < g->rules[i].termsInExpansion; k++) { // iterate through the terms in the expansion to the right of the non-terminal
// 
//                if (g->rules[i].expansion[k] < 58) { // found terminal, add it to follow set if it doesn't already exist in the follow
//                   int add = 1;
//                   for (int p = 0; p < ff->numFollow; p++) {
//                      if (ff->follow[p] == g->rules[i].expansion[k]) { add = 0; break; }
//                   }
//                   if (add) ff->follow[ff->numFollow++] = g->rules[i].expansion[k];
//                   gotTerminal = 1;
//                   break;
//                }
//                else { // found non-terminal
//                   firstAndFollow* other = &fS->sets[g->rules[i].expansion[k] - PROGRAM];
//                   for (int q = 0; q < other->numFirst; q++) { // iterate over the non terminal's first set
//                      int add = 1;
//                      for (int m = 0; m < ff->numFollow; m++) { // iterate over all the elements in the follow set of the current non terminal
//                         if (ff->follow[m] == other->first[q] || other->first[q] == EPS /*||  other->first[q] == TK_EOF check! */) { add = 0; break; } // check if term in first already exists in follow, and add it if not
//                      }
//                      if (add && other->first[q] != EPS) ff->follow[ff->numFollow++] = other->first[q];
//                   }
//                   if (! other->hasEps) break; // if the current non terminal's first set does not have a epsilon, then stop follow calculation for that rule
//                }
// 
//             } // k reached the end of expansion
// 
//             if (! gotTerminal) {
//                k = k - 1;
//                if (g->rules[i].nonTerminal != g->rules[i].expansion[k]) { // if the non terminal whose follow we wish to calculate appears as the last term in a rule's expansion, we skip this part
//                   firstAndFollow* lhs = &fS->sets[g->rules[i].nonTerminal - PROGRAM];
//                   if ((k == g->rules[i].termsInExpansion - 1 && nt == g->rules[i].expansion[k]) || fS->sets[g->rules[i].expansion[k] - PROGRAM].hasEps) {
//                      for (int q = 0; q < lhs->numFollow; q++) { // iterate over the LHS non terminal's follow set
//                         int add = 1;
//                         printf("%s >>> %s\n", getStringOf(lhs->nonTerminal), getStringOf(lhs->follow[q]));
//                         for (int m = 0; m < ff->numFollow; m++) { // iterate over all the elements in the follow set of the current non terminal
//                            if (ff->follow[m] == lhs->follow[q]) { add = 0; break; } // check if term in first already exists in follow, and add it if not
//                         }
//                         if (add) ff->follow[ff->numFollow++] = lhs->follow[q];
//                         printf("%s > %s", getStringOf(ff->nonTerminal), getStringOf(ff->follow[ff->numFollow-1]));
//                      }
//                   }
//                }
//             }
//          }
//       }
//    }

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

   // printf("first and follow sets\n");
   // for(int i=0;i<ff.numberOfFFS; i++) {
   //    if (0 == ff.sets[i].numFirst) continue;

   //    printf("%s =", getStringOf(ff.sets[i].nonTerminal));
   //    for (int j=0; j<ff.sets[i].numFirst; j++) {
   //       printf(" %s", getStringOf(ff.sets[i].first[j]));
   //    }
   //    printf("\n");
   // }

   // printf("\n\n-------------------------------\n\n");

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
