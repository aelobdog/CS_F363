#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "lexerDef.h"
#include "parser.h"
#include "parserDef.h"

int main(int argc, char* argv[]) {
    printf("FIRST and FOLLOW set automated\n");
    printf("Both lexical and syntax analysis modules implemented\n");
    printf("\n\n");

    int choice;
    if (argc != 2) { printf("USAGE: driver SOURCE_FILE\nExpecting 1 argument, got %d\n", argc - 1); exit(1); }

    char* filename = argv[1];
    printf("filename : %s\n", filename);

    int eof;
    ffSets ff;
    twinBuffer b;
    FILE* source;
    tokenList tList;
    parseTable pTable;
    parseStack pStack;
    hashTableEntry globalHashTable[HASHTABLESIZE];

    choice = 100; // just so that the while runs atleast one time

    while(choice) {
        printf("Options:\n");
        printf("\t0. Exit\n");
        printf("\t1. Print the comment free code\n");
        printf("\t2. Print the tokens\n");
        printf("\t3. Parse source code and print the parse tree\n");
        printf("\t4. Print time taken for stage 1 (lexing and parsing)\n");
        printf("Choice : ");
        scanf("%d", &choice);
        switch (choice) {
            case 0: break;

            case 1:
                consolePrintNoComments(filename);
                break;

            case 2:
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
                getAndPrintTokenList(&b, globalHashTable, tList.list);
                break;

            case 3: 
                memset(&pTable, 0, sizeof(pTable));
                memset(&ff, 0, sizeof(ff));
                memset(&pStack, 0, sizeof(pStack));
                memset(&tList, 0, sizeof(tList));
                gram g = readGram();
                printf("[COMPLETED] : GRAMMAR GENERATION FROM GRAMMAR FILE.\n");
                computeFirsts(&g, &ff);
                printf("[COMPLETED] : COMPUTATION OF FIRST SETS.\n");
                computeFollows(&g, &ff);
                printf("[COMPLETED] : COMPUTATION OF FOLLOW SETS.\n");
                populateParseTable(&pTable, &g, &ff);
                printf("[COMPLETED] : CREATION OF PARSE TABLE.\n");
                initStack(&pStack);
                initLexerDefaults(filename, source, &b, &eof, globalHashTable, &tList);
                printf("[COMPLETED] : LEXICAL ANALYSIS.\n");
                parseTreeNode* pTree = predictiveParse(&pStack, &pTable, &tList, &ff);
                printf("[COMPLETED] : SYNTAX ANALYSIS.\n");
                printf("[COMPLETED] : CREATION OF PARSE TREE.\n\n");
                printf("PARSE TREE:\n");
                printParseTree(pTree);

            break;
            
            case 4: break;
        //     case 4: clock_t start_time, end_time;
        //             double total_CPU_time, total_CPU_time_in_seconds;
        //             start_time = clock();
        //             // invoke your lexer and parser here
        //             end_time = clock();
        //             total_CPU_time = (double) (end_time - start_time);
        //             total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
        //             // Print both total_CPU_time and total_CPU_time_in_seconds
        }
    }
    return 0;
}
