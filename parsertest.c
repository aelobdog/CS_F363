#include "parser.h"
#include "parserDef.h"
#include <string.h>

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
   // for (int i=0; i<g.numberOfRules; i++) printRule(&g.rules[i]);
   // printf("--------------\n\n");

   computeFirsts(&g, &ff);

   // printf("First Sets\n");
   // for (int i=0; i < ff.numberOfFFS; i++) {
   //    printf("%s = { ", getStringOf(ff.sets[i].nonTerminal));
   //    for (int j = 0; j < ff.sets[i].numFirst; j++) {
   //       printf("%s ", getStringOf(ff.sets[i].first[j]));
   //    }
   //    printf("}\n");
   // }

   computeFollows(&g, &ff);

   // printf("Follow Sets\n");
   // for (int i=0; i < ff.numberOfFFS; i++) {
   //    printf("%s = { ", getStringOf(ff.sets[i].nonTerminal));
   //    for (int j = 0; j < ff.sets[i].numFollow; j++) {
   //       printf("%s ", getStringOf(ff.sets[i].follow[j]));
   //    }
   //    printf("}\n");
   // }

   populateParseTable(&pTable, &g, &ff);
   initStack(&pStack);

   initLexerDefaults("t6.txt", source, &b, &eof, globalHashTable, &tList);

   // printf("\n\n\n");

   parseTreeNode* pTree = predictiveParse(&pStack, &pTable, &tList, &ff);

   printf("DONE (stack length = %d, stack top : %s)", pStack.top, pStack.top ? getStringOf(getStackTop(&pStack)) : "(null)");

   printf("Printing parse tree\n");

   printParseTree(pTree);

   // for (int i=0; i<=pStack.top; i++) printf("%10s  ", getStringOf(pStack.stack[i]));
   // printf("\n");

   // termType t;

   // t = popStackTop(&pStack);
   // for (int i=0; i<=pStack.top; i++) printf("%10s  ", getStringOf(pStack.stack[i]));
   // printf("\n");

   // pushStackTop(&pStack, t, &pTable, &tList);
   // for (int i=0; i<=pStack.top; i++) printf("%10s  ", getStringOf(pStack.stack[i]));
   // printf("\n");

   return 0;
}