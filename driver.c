#include <stdio.h>
#include "src/lexer/lexer.h"
#include "src/lexer/lexerDef.h"

int main() {
    int choice;
    char filename[32] = "t1.txt";

    hashTableEntry globalHashTable[HASHTABLESIZE];
    twinBuffer b;
    FILE* source;
    token tokenList[2500];
    int eof;

    choice = 100;
    printf("Options:\n");
    printf("\t0. Exit\n");
    printf("\t1. Print the comment free code\n");
    printf("\t2. Print the result of tokenizing a source file\n");
    printf("\t3. TODO: TIMING\n");
    printf("\t3. TODO: TIMING\n");

    while(choice) {
        printf("Choice : ");
        // scanf("%d", &choice);
        choice =2 ;
        switch (choice) {
            case 0: break;

            case 1:
                printf("source file : ");
                scanf("%s", filename);
                consolePrintNoComments(filename);
                break;
            case 2:
                // printf("source file : ");
                // scanf("%s", filename)
                source = fopen(filename, "r");
                if (!source) perror("FILE READ ERROR : ");

                source = loadBuffer(b.buffer1, source, &eof);
                if (!eof) source = loadBuffer(b.buffer2, source, &eof);

                b.fBuf = b.buffer1;
                b.lbBuf = b.buffer1;
                b.forward = b.fBuf;
                b.lexemeBegin = b.lbBuf;
                b.currentLine = 1;

                initGlobalHashTable(globalHashTable);
                getAndPrintTokenList(&b, globalHashTable, tokenList);
                return 0;

                // int i;
                // for (i=0; tokenList[i].type != DOLLAR; i++) prettyToken(tokenList[i]);
                // prettyToken(tokenList[i]);
                // break;

            case 3: break;
        }
    }
    return 0;
}
