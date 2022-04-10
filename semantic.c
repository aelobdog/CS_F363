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

void addActOrRedef(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ACTUALORREDEFINED);
    
    if(ptn->leftChild->tokenInfo.tokenType == TYPEDEFINITION)
        addTypeDef(ptn->leftChild, ast, depth);
    
    else if(ptn->leftChild->tokenInfo.tokenType == DEFINETYPESTMT)
        addDefTypeStmt(ptn->leftChild, ast, depth);
}


// ------ CHECK LATER --------
void addRuid(parseTreeNode *ptn, astNode** ast, int depth) {
    astNode** iter = &(*ast)->leftChild;
    if (iter == NULL) {
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_RUID;
        (*iter)->depth = depth;
    } else {
        for (; *iter != NULL; iter = &((*iter)->rightSibling));
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_RUID;
        (*iter)->depth = depth;
    }
}
// ------------------------------

void addFieldDefs(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FIELDDEFINITIONS);
    astNode** iter = addNode(ast, FIELDDEFINITIONS, depth);

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case FIELDDEFINITION:
                addFieldDef(temp, iter, depth + 1);
                break;

            case MOREFIELDS:
                addMoreFields(temp, iter, depth + 1);
                break;
            
            default: break;
        }
    }
}

void addFieldDef(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FIELDDEFINITION);
    
    if(ptn->leftChild->rightSibling->tokenInfo.tokenType == FIELDTYPE)
        addFieldType(ptn->leftChild, ast, depth);
}

void addFieldType(parseTreeNode *ptn, astNode** ast, int depth) {
    // ----- ATTRIBUTES ---------------
}

void addMoreFields(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, MOREFIELDS);
    astNode** iter = addNode(ast, MOREFIELDS, depth);

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case FIELDDEFINITION:
                addFieldDef(temp, iter, depth + 1);
                break;

            case MOREFIELDS:
                addMoreFields(temp, iter, depth + 1);
                break;
            
            default: break;
        }
    }
}

void addTypeDef(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, TYPEDEFINITION);
    astNode** iter = addNode(ast, TYPEDEFINITION, depth);

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case TK_RUID:
                addRuid(temp, iter, depth + 1);
                break;

            case FIELDDEFINITIONS:
                addFieldDefs(temp, iter, depth + 1);
                break;
            
            default: break;
        }
    }
}

void addTypeDefs(parseTreeNode *ptn, astNode** ast, int depth) { 
    verify(ptn, TYPEDEFINITIONS);
    astNode** iter = addNode(ast, TYPEDEFINITIONS, depth);

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case ACTUALORREDEFINED:
                addActOrRedef(temp, iter, depth + 1);
                break;

            case TYPEDEFINITIONS:
                addTypeDefs(temp, iter, depth + 1);
                break;
            
            default: break;
        }
    }
}

void addDecls(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, DECLARATIONS);
    astNode** iter = addNode(ast, DECLARATIONS, depth);

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case DECLARATION:
                addDecl(temp, iter, depth + 1);
                break;

            case DECLARATIONS:
                addDecls(temp, iter, depth + 1);
                break;
            
            default: break;
        }
    }
}

void addOtherStmts(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OTHERSTMTS);
    astNode** iter = addNode(ast, OTHERSTMTS, depth);

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case STMT:
                addStmt(temp, iter, depth + 1);
                break;

            case OTHERSTMTS:
                addOtherStmts(temp, iter, depth + 1);
                break;
            
            default: break;
        }
    }
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

void addInpPar(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, INPUT_PAR);
    //astNode** iter = addNode(ast, INPUT_PAR, depth);

    addParList(iter, ast, depth);
}

void addOutPar(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OUTPUT_PAR);
    //astNode** iter = addNode(ast, OUTPUT_PAR, depth);

    addParList(iter, ast, depth);
}

void addParList(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, PARAMETER_LIST);
    astNode** iter = addNode(ast, PARAMETER_LIST, depth);

    for (; iter != NULL; iter = iter->rightSibling) {
        if (iter->tokenInfo.tokenType == TK_ID) {
            addId(iter, ast, depth + 1);
        } else if (iter->tokenInfo.tokenType == REMAINING_LIST) {
            addRemList(iter, ast, depth + 1);
        } 
    }
}

void addRemList(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, REMAINING_LIST);

    addParList(ptn->leftChild, iter, depth);
}

void addFunction(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FUNCTION);
    astNode** iter = addNode(ast, FUNCTION, depth);

    for (; iter != NULL; iter = iter->rightSibling) {
        if (iter->tokenInfo.tokenType == INPUT_PAR) {
            addInpPar(iter, ast, depth + 1);
        } else if (iter->tokenInfo.tokenType == OUTPUT_PAR) {
            addOutPar(iter, ast, depth + 1);
        } else if (iter->tokenInfo.tokenType == STMTS) {
            addStmts(iter, ast, depth + 1);
        }
    }
}

void addOtherFn(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OTHERFUNCTION);
    astNode** iter = addNode(ast, OTHERFUNCTION, depth);

    for (; iter != NULL; iter = iter->rightSibling) {
        if (iter->tokenInfo.tokenType == FUNCTION) {
            addFunction(iter, ast, depth + 1);
        } else if (iter->tokenInfo.tokenType == OTHERFUNCTION) {
            addOtherFn(iter, ast, depth + 1);
        }
    }
    
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
