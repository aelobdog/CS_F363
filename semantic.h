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
void printAstNode(astNode* root);

astNode* makeAST(parseTreeNode*, int);

void addId(parseTreeNode *ptn, astNode** ast, int depth);
void addMoreIds(parseTreeNode *ptn, astNode** ast, int depth);
void addIdList(parseTreeNode *ptn, astNode** ast, int depth);
void addOptRet(parseTreeNode *ptn, astNode** ast, int depth);
void addTypeDefs(parseTreeNode *ptn, astNode** ast, int depth);
void addDecls(parseTreeNode *ptn, astNode** ast, int depth);
void addOtherStmts(parseTreeNode *ptn, astNode** ast, int depth);
void addRetStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addStmts(parseTreeNode *ptn, astNode** ast, int depth);
void addOtherFn(parseTreeNode *ptn, astNode** ast, int depth);
void addMainFn(parseTreeNode *ptn, astNode** ast, int depth);
void mkProgram(parseTreeNode *ptn, astNode** ast);

#endif
