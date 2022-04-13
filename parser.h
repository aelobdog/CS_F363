// Group number 13
// Ashwin Kiran Godbole 2018B5A70423P
// Samarth Krishna Murthy 2018B2A70362P

#ifndef PARSER_H
#define PARSER_H
#include "parserDef.h"

gram readGram();
void computeFirsts(gram*, ffSets*);
void computeFollows(gram*, ffSets*);
void follow(termType, ffSets*, int, gram*);
int first(termType, ffSets*, int, gram*);
void populateParseTable(parseTable*, gram*, ffSets*);

// initialize the stack with <program> <$>
void initStack(parseStack*);

// returns the element at the top of the stack (without any modifications to the stack)
termType getStackTop(parseStack*);

// returns the element at the top of the stack and sets the stack's top to the new top 
// of the stack
termType popStackTop(parseStack*);

// pushes the expansion of the production rule corresponding to 'termType' from the
// 'parseTable' and sets the stack's top to the top of the stack.
prodRule* pushStackTop(parseStack*, termType, int, parseTable*, tokenList*);

// parses the given code using a stack and tokenstream by looking at rules from the parseTable
parseTreeNode* predictiveParse(parseStack*, parseTable*, tokenList*, ffSets*);

// recover from errors by checking the syn set (syn set = follow set)
void recoverFromError(termType, ffSets*, parseStack*, tokenList*);

// recover from errors by checking the syn set (syn set = follow and first set)
void recoverFromErrorNew(termType, ffSets*, parseStack*, tokenList*);

// create a parse tree with one node (set to PROGRAM)
parseTreeNode* initParseTree();

// creates a parse tree node for a non terminal of specified type
parseTreeNode* buildParseTreeNodeFromType(termType, int);
// creates a parse tree node for a terminal for the specified token
parseTreeNode* buildParseTreeNodeFromToken(token*, int);

// helper functions to print parseTree on console
void printParseTreeNode(parseTreeNode*);
void printParseTree(parseTreeNode*);

// helperFunctions to write the parse treeInorder to the console
void fprintParseTreeNode(parseTreeNode*);
void fprintParseTree(parseTreeNode*);

// helper functions to help in adding nodes to the parse tree
void addTerminalToParseTreeAt(parseTreeNode**, int, token*);
void addNonTerminalToParseTreeAt(parseTreeNode**, int, termType);

void getpTreeSize(parseTreeNode*, long*);

#endif
