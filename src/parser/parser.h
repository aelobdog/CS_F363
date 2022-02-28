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
prodRule* pushStackTop(parseStack*, termType, parseTable*, tokenList*);

//parses the given code using a stack and tokenstream by looking at rules from the parseTable
parseTreeNode* predictiveParse(parseStack*, parseTable*, tokenList*);

void initParseTree(parseTreeNode*);

parseTreeNode* buildParseTreeNodeFromType(termType);

parseTreeNode* buildParseTreeNodeFromToken(token*);

void buildTreeFromRuleAt(parseTreeNode*, prodRule*);



#endif
