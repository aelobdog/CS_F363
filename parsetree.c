#include <stdio.h>
#include "src/parser/parser.h"
#include "src/parser/parserDef.h"

int main() {
   parseTreeNode* root = initParseTree();

   addNonTerminalToParseTreeAt(&root, 1, OTHERFUNCTION);
   addNonTerminalToParseTreeAt(&root, 1, MAINFUNCTION);
   addNonTerminalToParseTreeAt(&root->leftChild->rightSibling, 2, TK_WHILE);
   addNonTerminalToParseTreeAt(&root->leftChild->rightSibling, 2, TK_IF);
   addNonTerminalToParseTreeAt(&root->leftChild->rightSibling, 2, TK_ENDIF);

   // root->leftChild = buildParseTreeNodeFromType(OTHERFUNCTION);
   // root->leftChild->rightSibling = buildParseTreeNodeFromType(MAINFUNCTION);
   // root->leftChild->rightSibling->leftChild = buildParseTreeNodeFromType(TK_WHILE);
   // root->leftChild->rightSibling->leftChild->rightSibling = buildParseTreeNodeFromType(TK_IF);
   // root->leftChild->rightSibling->leftChild->rightSibling->rightSibling = buildParseTreeNodeFromType(TK_ENDIF);


   printParseTree(root, 0);
   return 0;
}