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

astNode** addNode(astNode** ast, termType T, int depth);
void addFieldId(parseTreeNode *ptn, astNode** ast, int depth);
void addInpPar(parseTreeNode *ptn, astNode** ast, int depth);
void addOutPar(parseTreeNode *ptn, astNode** ast, int depth);
void addRemList(parseTreeNode *ptn, astNode** ast, int depth);
void addFunction(parseTreeNode *ptn, astNode** ast, int depth);
void addParList(parseTreeNode *ptn, astNode** ast, int depth, int createNode);
void addFunId(parseTreeNode *ptn, astNode** ast, int depth);
void addActOrRedef(parseTreeNode *ptn, astNode** ast, int depth);
void addRuid(parseTreeNode *ptn, astNode** ast, int depth);
void addFieldDefs(parseTreeNode *ptn, astNode** ast, int depth);
void addFieldDef(parseTreeNode *ptn, astNode** ast, int depth);
void addMoreFields(parseTreeNode *ptn, astNode** ast, int depth);
void addTypeDef(parseTreeNode *ptn, astNode** ast, int depth);
void addDefTypeStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addId(parseTreeNode *ptn, astNode** ast, int depth);
void addMoreIds(parseTreeNode *ptn, astNode** ast, int depth);
void addIdList(parseTreeNode *ptn, astNode** ast, int depth);
void addOptRet(parseTreeNode *ptn, astNode** ast, int depth);
void addTypeDefs(parseTreeNode *ptn, astNode** ast, int depth, int createNode);
void addDecls(parseTreeNode *ptn, astNode** ast, int depth, int createNode);
void addOtherStmts(parseTreeNode *ptn, astNode** ast, int depth);
void addRetStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addStmts(parseTreeNode *ptn, astNode** ast, int depth);
void addOtherFn(parseTreeNode *ptn, astNode** ast, int depth, int createNode);
void addMainFn(parseTreeNode *ptn, astNode** ast, int depth);
void addDecl(parseTreeNode *ptn, astNode** ast, int depth);
void mkProgram(parseTreeNode *ptn, astNode** ast);
#endif
