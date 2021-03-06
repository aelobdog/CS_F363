// Group number 13
// Ashwin Kiran Godbole 2018B5A70423P
// Samarth Krishna Murthy 2018B2A70362P

#ifndef SEMANTIC_h
#define SEMANTIC_h
#include "semanticDef.h"

void makeConTypeTable(astNode* ast, conTypeWrapper* cwrap);
void printConTypeTable(conTypeWrapper* cwrap);
void printSymbolTable(symbolTable* sTable, conTypeWrapper* cwrap);
void makeSymbolTable(astNode* ast, symbolTable* globalTable, symbolTable localTables[50], int index, conTypeWrapper*);
void makeSymbolTables(astNode* ast, symbolTable* globalTable, symbolTable localTables[50], conTypeWrapper*);
// ----------------------------------------------------------------------------
void printAST(astNode* root);
void printAstNode(astNode* root);

void getAstSize(astNode*, long*, long*);
astNode* makeAST(parseTreeNode*, int);
astNode** addNode(astNode** ast, termType T, int depth);
void addElsePart(parseTreeNode *ptn, astNode** ast, int depth);
int  addThenPart(parseTreeNode *ptn, astNode** ast, int depth);
void addBoolExp(parseTreeNode *ptn, astNode** ast, int depth);
void addOutputPars(parseTreeNode *ptn, astNode** ast, int depth);
void addInputPars(parseTreeNode *ptn, astNode** ast, int depth);
void addIterStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addIOStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addCondStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addFnCallStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addRNum(parseTreeNode *ptn, astNode** ast, int depth);
void addNum(parseTreeNode *ptn, astNode** ast, int depth);
void addTerm(parseTreeNode *ptn, astNode** ast, int depth);
void addFactor(parseTreeNode *ptn, astNode** ast, int depth);
void addExpPrime(parseTreeNode *ptn, astNode** ast, int depth);
void addTermPrime(parseTreeNode *ptn, astNode** ast, int depth);
void addLPO(parseTreeNode *ptn, astNode** ast, int depth);
void addHPO(parseTreeNode *ptn, astNode** ast, int depth);
void addVar(parseTreeNode *ptn, astNode** ast, int depth);
void addArithExp(parseTreeNode *ptn, astNode** ast, int depth);
void addMoreExp(parseTreeNode *ptn, astNode** ast, int depth);
void addOneExp(parseTreeNode *ptn, astNode** ast, int depth);
void addConstVar(parseTreeNode *ptn, astNode** ast, int depth);
void addAssignStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addStmt(parseTreeNode *ptn, astNode** ast, int depth);
void addSingOrRecId(parseTreeNode *ptn, astNode** ast, int depth);
void addFieldId(parseTreeNode *ptn, astNode** ast, int depth, termType type, char* name);
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
void addId(parseTreeNode *ptn, astNode** ast, int depth, termType datatype, char* ruid, int isGlobal);
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
// ----------------------------------------------------------------------------

// symbolTable makeTable(astNode*, symbolTable* parentTable);

// ----------------------------------------------------------------------------

#endif
