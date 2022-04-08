#ifndef SEMANTIC_h
#define SEMANTIC_h
#include "semanticDef.h"

// create a new symbol table (the argument taken is a pointer to
// the table that the new table is inside of - scope wise)
symbolTable* mkTable(parseTreeNode*, symbolTable* previous);

astNode* mkAST(parseTreeNode *ptn, int depth);

void enter(symbolTable*, char*, dataType, ul);

int addWidth(symbolTable*, ul);

void printAST(astNode* root);

astNode* makeAST(parseTreeNode*, int);

#endif