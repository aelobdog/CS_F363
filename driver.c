// Group number 13
// Ashwin Kiran Godbole 2018B5A70423P
// Samarth Krishna Murthy 2018B2A70362P

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "lexer.h"
#include "lexerDef.h"
#include "parser.h"
#include "parserDef.h"
#include "semantic.h"
#include "semanticDef.h"

int main (int argc, char* argv[]) {
    printf("FIRST and FOLLOW set automated\n");
    printf("Both lexical and syntax analysis modules implemented\n");
    printf("\n\n");

    int choice;
    if (argc < 2) { printf("USAGE: ./stage1exe SOURCE_FILE\nExpecting 1 argument, got %d\n", argc - 1); exit(1); }

    char* filename = argv[1];
    printf("filename : %s\n", filename);

    int eof;
    ffSets ff;
    twinBuffer b;
    tokenList tList;
    gram g;
    parseTable pTable;
    parseStack pStack;
    hashTableEntry globalHashTable[HASHTABLESIZE];
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    struct timeval t1, t2;

    // choice = 100; // just so that the while runs atleast one time
    choice = 3;

    while(choice) {
        // printf("Options:\n");
        // printf("\t0. Exit\n");
        // printf("\t1. Print the comment free code\n");
        // printf("\t2. Print the tokens\n");
        // printf("\t3. Parse source code and print the parse tree\n");
        // printf("\t4. Print time taken for stage 1 (lexing and parsing)\n");
        // printf("Choice : ");
        // scanf("%d", &choice);
        switch (choice) {
            case 0: break;

            case 1:
                consolePrintNoComments(filename);
                printf("\n");
                break;

            case 2:
                b.source = fopen(filename, "r");
                if (! b.source) perror("FILE READ ERROR : ");

                b.source = loadBuffer(b.buffer1, b.source, &eof);
                if (!eof)  b.source = loadBuffer(b.buffer2, b.source, &eof);

                b.fBuf = b.buffer1;
                b.lbBuf = b.buffer1;
                b.forward = b.fBuf;
                b.lexemeBegin = b.lbBuf;
                b.currentLine = 1;

                initGlobalHashTable(globalHashTable);
                token T;
                T = getToken(&b, globalHashTable);
                while (T.type != DOLLAR) {
                    prettyToken(T);
                    
                    T = getToken(&b, globalHashTable);
                }
                // getAndPrintTokenList(&b, globalHashTable, tList.list);
                fclose(b.source);
                break;

            case 3: 
                // if (argc != 3) { printf("USAGE : ./stage1exe SOURCE_FILE PARSETREE_OUTFILE\n"); continue; }
                memset(&pTable, 0, sizeof(pTable));
                memset(&ff, 0, sizeof(ff));
                memset(&pStack, 0, sizeof(pStack));
                memset(&tList, 0, sizeof(tList));
                g = readGram();
                //printf("[COMPLETED] : GRAMMAR GENERATION FROM GRAMMAR FILE.\n");
                computeFirsts(&g, &ff);
                //printf("[COMPLETED] : COMPUTATION OF FIRST SETS.\n");
                computeFollows(&g, &ff);
                //printf("[COMPLETED] : COMPUTATION OF FOLLOW SETS.\n");
                populateParseTable(&pTable, &g, &ff);
                //printf("[COMPLETED] : CREATION OF PARSE TABLE.\n");
                initStack(&pStack);
                initLexerDefaults(filename, &b, &eof, globalHashTable, &tList);
                //printf("[COMPLETED] : LEXICAL ANALYSIS.\n");
                parseTreeNode* pTree = predictiveParse(&pStack, &pTable, &tList, &ff);
                //printf("[COMPLETED] : SYNTAX ANALYSIS.\n");
                //printf("[COMPLETED] : CREATION OF PARSE TREE.\n\n");
                printParseTree(pTree);
                printf("\n\n");
                //FILE* file = fopen(argv[2], "w");
                //fprintParseTree(pTree, file);
                //fclose(file);
                //fclose(b.source);
                //printf("PARSE TREE WRITTEN TO FILE: %s\n",argv[2]);

                // modifiying this part ----------
                astNode* ast = makeAST(pTree, 0);// mkAST(pTree, 0);
                // printAstNode(ast);
                // printAstNode(ast->leftChild);
                printf("\n\n");
                printAST(ast);
                // -------------------------------
                choice = 0;
                exit(0);
            
            case 4: 
                memset(&pTable, 0, sizeof(pTable));
                memset(&ff, 0, sizeof(ff));
                memset(&pStack, 0, sizeof(pStack));
                memset(&tList, 0, sizeof(tList));
                start_time = clock();
                gettimeofday(&t1, NULL);
                    g = readGram();
                    computeFirsts(&g, &ff);
                    computeFollows(&g, &ff);
                    populateParseTable(&pTable, &g, &ff);
                    initStack(&pStack);
                    initLexerDefaults(filename, &b, &eof, globalHashTable, &tList);
                    predictiveParse(&pStack, &pTable, &tList, &ff);
                gettimeofday(&t2, NULL);
                end_time = clock();
                total_CPU_time = (double) (end_time - start_time);
                total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
                printf("Total CPU time : %lf\n", total_CPU_time);
                printf("Total CPU time in seconds : %lf\n", total_CPU_time_in_seconds);
                double time = (t2.tv_usec - t1.tv_usec);
                printf("Total time taken [using gettimeofday() from sys/time.h] : %lfms (%lfs)\n", time/1000, time/1000000);
                break;

            default: printf("Please enter a value between 0-4");
                continue;
        }
    }
    return 0;
}
