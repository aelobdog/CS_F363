#include <stdio.h>
#include "src/parser/parser.h"
#include "src/parser/parserDef.h"

int main() {
   parseTreeNode* root = initParseTree();

   printf("%d, %p\n", root->isTerminal, root);

   root->leftChild = buildParseTreeNodeFromType(OTHERFUNCTION);
   root->leftChild->rightSibling = buildParseTreeNodeFromType(MAINFUNCTION);
   root->leftChild->rightSibling->leftChild = buildParseTreeNodeFromType(TK_WHILE);
   root->leftChild->rightSibling->leftChild->rightSibling = buildParseTreeNodeFromType(TK_IF);
   root->leftChild->rightSibling->leftChild->rightSibling->rightSibling = buildParseTreeNodeFromType(TK_ENDIF);


   printParseTree(root, 0);
   return 0;
}