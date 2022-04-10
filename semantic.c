#include "semantic.h"
#include "lexer.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "semanticDef.h"
#include <stdlib.h>

void printAstNode(astNode* node) {
   if(node==NULL) printf("null\n");
   printf("↪ %s\n", getStringOf(node->nodeName));
}

int verify(parseTreeNode* ptn, termType T) {
    printf("verifying %s\n", getStringOf(T));
    if (ptn->tokenInfo.tokenType != T) {
        printf("ERROR: In parse tree, expected %s, got %s\n",
                getStringOf(T),
                getStringOf(ptn->tokenInfo.tokenType));

        return 0;
    }
    return 1;
}

astNode** addNode(astNode** ast, termType T, int depth) {
    astNode** iter = &(*ast)->leftChild;
    if (iter == NULL) {
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = T;
        (*iter)->depth = depth;

    } else {
        for (; *iter != NULL; iter = &((*iter)->rightSibling));
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = T;
        (*iter)->depth = depth;
    }

    return iter;
}


void addId(parseTreeNode *ptn, astNode** ast, int depth) {
    astNode** iter = &(*ast)->leftChild;
    if (iter == NULL) {
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_ID;
        (*iter)->depth = depth;
    } else {
        for (; *iter != NULL; iter = &((*iter)->rightSibling));
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_ID;
        (*iter)->depth = depth;
    }
}

void addMoreIds(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, MORE_IDS);
    // if there is a node for moreids, it means that there is atleast one more
    // id inside it. So we can simply assume that the node has 2 children and
    // the right child is the idList.

    addIdList(ptn->leftChild->rightSibling, ast, depth);
}

void addIdList(parseTreeNode *ptn, astNode** ast, int depth) { 
    verify(ptn, IDLIST);
    
    // astNode** iter = &(*ast)->leftChild;
    // if (iter == NULL) {
    //     *iter = (astNode*) malloc(sizeof(astNode));
    //     (*iter)->nodeName = IDLIST;
    //     (*iter)->depth = depth;

    // } else {
    //     for (; *iter != NULL; iter = &((*iter)->rightSibling));
    //     *iter = (astNode*) malloc(sizeof(astNode));
    //     (*iter)->nodeName = IDLIST;
    //     (*iter)->depth = depth;
    // }
    
    // the left child has to be a terminal corresponding to an identifier
    // using a function to add in the identifier because it will make making
    // the symbol table stuff easier later.
    addId(ptn->leftChild, ast, depth);
    if (ptn->leftChild->rightSibling != NULL) addMoreIds(ptn->leftChild->rightSibling, ast, depth);
}

void addOptRet(parseTreeNode *ptn, astNode** ast, int depth) { 
    verify(ptn, OPTIONALRETURN);
    // If there is a node in the parse tree for optional return, it means
    // that there is atleast one return value. We don't add a node for 
    // optional return because it is useless with respect to the AST

    if (!ptn->leftChild->rightSibling->isTerminal)
        addIdList(ptn->leftChild->rightSibling, ast, depth);

    // parseTreeNode* temp = ptn->leftChild;
    // for(; temp != NULL; temp = temp->rightSibling) {
    //     if (!temp->isTerminal && temp->tokenInfo.tokenType == IDLIST) {
    //         addIdList(temp, ast, depth);
    //     }
    // }
}

void addTypeDefs(parseTreeNode *ptn, astNode** ast, int depth) { 
    verify(ptn, TYPEDEFINITIONS);
}

void addDecls(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, DECLARATIONS);
}

void addOtherStmts(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OTHERSTMTS);
}

void addRetStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, RETURNSTMT);
    astNode** iter = addNode(ast, RETURNSTMT, depth);
    
    if (!ptn->leftChild->rightSibling->isTerminal) 
        addOptRet(ptn->leftChild->rightSibling, iter, depth + 1);
}

void addStmts(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, STMTS);
    astNode** iter = addNode(ast, STMTS, depth);

    int count = 0;
    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case TYPEDEFINITIONS:
                addTypeDefs(temp, iter, depth + 1);
                break;

            case DECLARATIONS:
                addDecls(temp, iter, depth + 1);
                break;

            case OTHERSTMTS:
                addOtherStmts(temp, iter, depth + 1);
                break;

            case RETURNSTMT:
                addRetStmt(temp, iter, depth + 1);
                break;
            
            default: break;
        }
    }
}

void addOtherFn(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OTHERFUNCTION);
}

void addMainFn(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, MAINFUNCTION);
    astNode** iter = addNode(ast, MAINFUNCTION, depth);

    addStmts(ptn->leftChild->rightSibling, iter, depth + 1);
}

void mkProgram(parseTreeNode *ptn, astNode** ast) {
    verify(ptn, PROGRAM);
    
    *ast = (astNode*) malloc(sizeof(astNode));
    (*ast)->nodeName = PROGRAM;
    parseTreeNode* iter = ptn->leftChild;

    for (; iter != NULL; iter = iter->rightSibling) {
        if (iter->tokenInfo.tokenType == OTHERFUNCTION) {
            addOtherFn(iter, ast, 1);
        } else if (iter->tokenInfo.tokenType == MAINFUNCTION) {
            addMainFn(iter, ast, 1);
        }
    }
}

astNode* makeAST(parseTreeNode* ptn, int depth) {
    astNode* ast;
    mkProgram(ptn, &ast);
    return ast;
}

void printAST(astNode* node) {
   astNode* sib = node;

   for (int j = 0; sib != NULL; j ++, sib = sib->rightSibling) {
      for(int i=0; i<sib->depth; i++) printf("   ");
      printAstNode(sib);
      if (sib->leftChild != NULL) {
         printAST(sib->leftChild);
      }
   }
}
