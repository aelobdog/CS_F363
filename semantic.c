// TODO:
// 
// maybe have different structs for leaves and internal nodes and have the ast use a 
// union with pointers to decide which to load ?
//
// addNode and addLeaf should be different ?
//
// the value of num/rnum is still in idPtr, change to number ?


#include "semantic.h"
#include "lexer.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "semanticDef.h"
#include <stdio.h>
#include <stdlib.h>

void
printAstNode(astNode* node) {
    if(node==NULL) printf("null\n");
    printf("↪ %s\n", getStringOf(node->nodeName));
}

int
verify(parseTreeNode* ptn, termType T) {
    printf("(%d)", T);
    printf("verifying %s\n", getStringOf(T));
    
    int Tch = ptn->isTerminal ? ptn->tokenInfo.tokenPtr->type : ptn->tokenInfo.tokenType;

    if (Tch != T) {
        printf("ERROR: In parse tree, expected %s, got %s\n",
                getStringOf(T),
                getStringOf(ptn->tokenInfo.tokenType));

        return 0;
    }
    return 1;
}

astNode**
addNode(astNode** ast, termType T, int depth) {
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

void
addId(parseTreeNode *ptn, astNode** ast, int depth) {
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

void
addFieldId(parseTreeNode *ptn, astNode** ast, int depth) {
    astNode** iter = &(*ast)->leftChild;
    if (iter == NULL) {
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_FIELDID;
        (*iter)->depth = depth;
    } else {
        for (; *iter != NULL; iter = &((*iter)->rightSibling));
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_FIELDID;
        (*iter)->depth = depth;
    }
}

void
addMoreIds(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, MORE_IDS);
    // if there is a node for moreids, it means that there is atleast one more
    // id inside it. So we can simply assume that the node has 2 children and
    // the right child is the idList.

    addIdList(ptn->leftChild->rightSibling, ast, depth);
}

void
addIdList(parseTreeNode *ptn, astNode** ast, int depth) { 
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

void
addOptRet(parseTreeNode *ptn, astNode** ast, int depth) { 
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

void
addDefTypeStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, DEFINETYPESTMT);
    // dont really know what to do with this... definetypestmt does not
    // seem like something that should appear in the AST, but seems more
    // like something to be used while typechecking... So leaving it blank
    // for now
}

void
addActOrRedef(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ACTUALORREDEFINED);

    if(ptn->leftChild->tokenInfo.tokenType == TYPEDEFINITION)
        addTypeDef(ptn->leftChild, ast, depth);

    else if(ptn->leftChild->tokenInfo.tokenType == DEFINETYPESTMT)
        addDefTypeStmt(ptn->leftChild, ast, depth);
}


void
addRuid(parseTreeNode *ptn, astNode** ast, int depth) {
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

void
addFunId(parseTreeNode *ptn, astNode** ast, int depth) {
    astNode** iter = &(*ast)->leftChild;
    if (iter == NULL) {
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_FUNID;
        (*iter)->depth = depth;
    } else {
        for (; *iter != NULL; iter = &((*iter)->rightSibling));
        *iter = (astNode*) malloc(sizeof(astNode));
        (*iter)->nodeName = TK_FUNID;
        (*iter)->depth = depth;
    }
}

void
addFieldDefs(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FIELDDEFINITIONS);
    astNode** iter = addNode(ast, FIELDDEFINITIONS, depth);

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling) {
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

void
addFieldDef(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FIELDDEFINITION);
    // add the required information to symbol table
    addFieldId(ptn->leftChild->rightSibling->rightSibling->rightSibling, ast, depth);
}

void
addMoreFields(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, MOREFIELDS);
    addFieldDef(ptn->leftChild, ast, depth);
    if(ptn->leftChild->rightSibling != NULL) 
        addMoreFields(ptn->leftChild->rightSibling, ast, depth);
}

void
addTypeDef(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, TYPEDEFINITION);
    astNode** iter = addNode(ast, TYPEDEFINITION, depth);

    addRuid(ptn->leftChild->rightSibling, iter, depth + 1);
    addFieldDefs(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
}

void
addTypeDefs(parseTreeNode *ptn, astNode** ast, int depth, int createNode) { 
    verify(ptn, TYPEDEFINITIONS);
    astNode** iter;

    if (createNode) iter = addNode(ast, TYPEDEFINITIONS, depth);
    else iter = ast;

    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling) {
        switch(temp->tokenInfo.tokenType) {
            case ACTUALORREDEFINED:
                addActOrRedef(temp, iter, depth + 1);
                break;

            case TYPEDEFINITIONS:
                addTypeDefs(temp, ast, depth, 0);
                break;

            default: break;
        }
    }
}

void
addDecl(parseTreeNode *ptn, astNode **ast, int depth) {
    verify(ptn, DECLARATION);
    // for now, a declaration is nothing more than dummy placeholder for an
    // identifier. No node is going to be created, and trust is being put in
    // the implementer of the symbol table to handle the ID properties properly
    parseTreeNode* temp = ptn->leftChild->rightSibling->rightSibling->rightSibling;
    addId(temp, ast, depth);
}

void
addDecls(parseTreeNode *ptn, astNode** ast, int depth, int createNode) {
    verify(ptn, DECLARATIONS);
    astNode** iter;
    if (createNode) iter = addNode(ast, DECLARATIONS, depth);
    else iter = ast;

    // the left child of a declarations node is always a declaration
    addDecl(ptn->leftChild, iter, depth + 1);

    if (ptn->leftChild->rightSibling != NULL)
        addDecls(ptn->leftChild->rightSibling, ast, depth, 0);

    // parseTreeNode* temp = ptn->leftChild;
    // for(; temp != NULL; temp = temp->rightSibling, count++) {
    //     switch(temp->tokenInfo.tokenType) {
    //         case DECLARATION:
    //             addDecl(temp, iter, depth + 1);
    //             break;

    //         case DECLARATIONS:
    //             addDecls(temp, iter, depth + 1);
    //             break;

    //         default: break;
    //     }
    // }
}

void
addMoreExp(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, MOREEXPANSIONS);
    addOneExp(ptn->leftChild, ast, depth);
    if (ptn->leftChild->rightSibling != NULL)
        addMoreExp(ptn->leftChild->rightSibling, ast, depth);
}

void
addOneExp(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ONEEXPANSION);
    addFieldId(ptn->leftChild->rightSibling, ast, depth);
}

void
addConstVar(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, CONSTRUCTEDVARIABLE);
    addOneExp(ptn->leftChild, ast, depth);
    if (ptn->leftChild->rightSibling != NULL)
        addMoreExp(ptn->leftChild->rightSibling, ast, depth);
}

void
addSingOrRecId(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, SINGLEORRECID);
    astNode** iter;
    if (ptn->leftChild->rightSibling != NULL) iter = addNode(ast, SINGLEORRECID, depth);
    else iter = ast;

    addId(ptn->leftChild, iter, depth + 1);
    
    if (iter != ast) {
        addConstVar(ptn->leftChild->rightSibling, iter, depth + 1);
    }
}

void
addTermPrime(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, TERMPRIME);
    astNode** iter;
    if (ptn->leftChild->rightSibling->rightSibling != NULL) {
        iter = addNode(
                ast,
                // getting the operator (watch out for dirty code!)
                ptn->leftChild->rightSibling->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type,
                depth);
    } else iter = ast;

    if (iter != ast) depth ++;
    addFactor(ptn->leftChild->rightSibling, iter, depth);

    if (iter != ast)
        addTermPrime(ptn->leftChild->rightSibling->rightSibling, iter, depth);
}

void
addExpPrime(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, EXPPRIME);
    astNode** iter;
    if (ptn->leftChild->rightSibling->rightSibling != NULL) {
        iter = addNode(
                ast,
                // getting the operator (watch out for dirty code!)
                ptn->leftChild->rightSibling->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type,
                depth);
    } else iter = ast;

    if (iter != ast) depth ++;

    addTerm(ptn->leftChild->rightSibling, iter, depth);
    
    if (iter != ast)
        addExpPrime(ptn->leftChild->rightSibling->rightSibling, iter, depth);
}

void
addRNum(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, TK_RNUM);
    addNode(ast, TK_RNUM, depth);
}

void
addNum(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, TK_NUM);
    addNode(ast, TK_NUM, depth);
}

void
addVar(parseTreeNode *ptn, astNode** ast, int depth) {
   verify(ptn, VAR);
   if (ptn->leftChild->isTerminal) {
        if (ptn->leftChild->tokenInfo.tokenPtr->type == TK_RNUM) {
            addRNum(ptn->leftChild, ast, depth);
        } else {
            addNum(ptn->leftChild, ast, depth);
        }
   } else {
        addSingOrRecId(ptn->leftChild, ast, depth);
   }
}

void
addFactor(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FACTOR);
    if (ptn->leftChild->isTerminal) 
        addArithExp(ptn->leftChild->rightSibling, ast, depth); // increase depth and make a node for this case ?
    else
        addVar(ptn->leftChild, ast, depth);
}

void
addTerm(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, TERM);
    astNode** iter;
    if (ptn->leftChild->rightSibling != NULL) {
        iter = addNode(
                ast,
                // getting the operator (watch out for dirty code!)
                ptn->leftChild->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type,
                depth);
    } else iter = ast;

    if (iter != ast) depth ++;

    addFactor(ptn->leftChild, iter, depth);

    if (iter != ast)
        addTermPrime(ptn->leftChild->rightSibling, iter, depth);
}

void
addArithExp(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ARITHMETICEXPRESSION);
    astNode** iter;
    // printf("%d\n", ptn->leftChild->rightSibling->leftChild->isTerminal); // the operator
    if (ptn->leftChild->rightSibling != NULL) {
        iter = addNode(
                ast,
                // getting the operator (watch out for dirty code!)
                ptn->leftChild->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type,
                depth);
    } else iter = ast;

    if (iter != ast) depth ++;

    addTerm(ptn->leftChild, iter, depth);
    
    if (iter != ast)
        addExpPrime(ptn->leftChild->rightSibling, iter, depth);
}

void
addAssignStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ASSIGNMENTSTMT);
    astNode** iter = addNode(ast, ASSIGNMENTSTMT, depth);
    addSingOrRecId(ptn->leftChild, iter, depth);
    addArithExp(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
}

void
addStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, STMT);
    parseTreeNode* temp = ptn->leftChild;

    for(; temp != NULL; temp = temp->rightSibling) {
        switch(temp->tokenInfo.tokenType) {
            case ASSIGNMENTSTMT: 
                addAssignStmt(temp, ast, depth);
                break;

            case ITERATIVESTMT:  
                break;

            case IOSTMT:         
                break;

            case FUNCALLSTMT:    
                break;

            default: break;
        }
    }
}

void
addOtherStmts(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OTHERSTMTS);
    addStmt(ptn->leftChild, ast, depth);
    if (ptn->leftChild->rightSibling != NULL)
        addOtherStmts(ptn->leftChild->rightSibling, ast, depth);
}

void
addRetStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, RETURNSTMT);
    astNode** iter = addNode(ast, RETURNSTMT, depth);

    if (!ptn->leftChild->rightSibling->isTerminal) 
        addOptRet(ptn->leftChild->rightSibling, iter, depth + 1);
}

void
addStmts(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, STMTS);
    astNode** iter = addNode(ast, STMTS, depth);

    int count = 0;
    parseTreeNode* temp = ptn->leftChild;
    for(; temp != NULL; temp = temp->rightSibling, count++) {
        switch(temp->tokenInfo.tokenType) {
            case TYPEDEFINITIONS:
                addTypeDefs(temp, iter, depth + 1, 1);
                break;

            case DECLARATIONS:
                addDecls(temp, iter, depth + 1, 1);
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

void
addInpPar(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, INPUT_PAR);
    addParList(ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling, ast, depth, 1);
}

void
addOutPar(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OUTPUT_PAR);
    addParList(ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling, ast, depth, 1);
}

void
addParList(parseTreeNode *ptn, astNode** ast, int depth, int createNode) {
    verify(ptn, PARAMETER_LIST);
    astNode** iter;
    if (createNode) iter = addNode(ast, PARAMETER_LIST, depth);
    else iter = ast;

    addId(ptn->leftChild->rightSibling, iter, depth + 1);
    if (ptn->leftChild->rightSibling->rightSibling != NULL)
        addRemList(ptn->leftChild->rightSibling->rightSibling, iter, depth);
}

void
addRemList(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, REMAINING_LIST);
    addParList(ptn->leftChild->rightSibling, ast, depth, 0);
}

void
addFunction(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FUNCTION);
    astNode** iter = addNode(ast, FUNCTION, depth);

    addFunId(ptn->leftChild, iter, depth + 1);
    addInpPar(ptn->leftChild->rightSibling, iter, depth + 1);
    if (! ptn->leftChild->rightSibling->rightSibling->isTerminal)
        addOutPar(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
    
    addStmts(ptn->leftChild->rightSibling->rightSibling->rightSibling, iter, depth + 1);
}

void
addOtherFn(parseTreeNode *ptn, astNode** ast, int depth, int createNode) {
    verify(ptn, OTHERFUNCTION);
    astNode** iter;
    if (createNode) iter = addNode(ast, OTHERFUNCTION, depth);
    else iter = ast;

    addFunction(ptn->leftChild, iter, depth + 1);
    if(ptn->leftChild->rightSibling != NULL)
        addOtherFn(ptn->leftChild->rightSibling, ast, depth, 0);
}

void
addMainFn(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, MAINFUNCTION);
    astNode** iter = addNode(ast, MAINFUNCTION, depth);

    addStmts(ptn->leftChild->rightSibling, iter, depth + 1);
}

void
mkProgram(parseTreeNode *ptn, astNode** ast) {
    verify(ptn, PROGRAM);

    *ast = (astNode*) malloc(sizeof(astNode));
    (*ast)->nodeName = PROGRAM;
    parseTreeNode* iter = ptn->leftChild;

    for (; iter != NULL; iter = iter->rightSibling) {
        if (iter->tokenInfo.tokenType == OTHERFUNCTION) {
            addOtherFn(iter, ast, 1, 1);
        } else if (iter->tokenInfo.tokenType == MAINFUNCTION) {
            addMainFn(iter, ast, 1);
        }
    }
}

astNode*
makeAST(parseTreeNode* ptn, int depth) {
    astNode* ast;
    mkProgram(ptn, &ast);
    return ast;
}

void
printAST(astNode* node) {
    astNode* sib = node;

    for (int j = 0; sib != NULL; j ++, sib = sib->rightSibling) {
        for(int i=0; i<sib->depth; i++) printf("   ");
        printAstNode(sib);
        if (sib->leftChild != NULL) {
            printAST(sib->leftChild);
        }
    }
}
