#ifndef PARSER_H
#define PARSER_H
#include "parserDef.h"

gram readGram();
void computeFirsts(gram*, ffSets*);
void computeFollows(gram*, ffSets*);
void follow(termType, ffSets*, int, gram*);
int first(termType, ffSets*, int, gram*);
void populateParseTable(parseTable*, gram*, ffSets*);

void initStack(parseStack*);
termType getStackTop(parseStack*);
termType popStackTop(parseStack*);
void pushStackTop(parseStack*, termType, parseTable*, tokenList*);


#endif
