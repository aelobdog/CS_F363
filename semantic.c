// Group number 13
// Ashwin Kiran Godbole 2018B5A70423P
// Samarth Krishna Murthy 2018B2A70362P


#include "semantic.h"
#include "lexer.h"
#include "lexerDef.h"
#include "parserDef.h"
#include "parser.h"
#include "semanticDef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
printAstNode(astNode* node) {
    if(node==NULL) printf("null\n");

    switch (node->nodeName) {
        case TK_RNUM:
            printf("%s [%lf] (parent : %s)\n", getStringOf(node->nodeName), node->leafInfo->value.rnum, node->nodeName != PROGRAM ? getStringOf(node->parent->nodeName) : "ROOT");
            break;

        case TK_NUM:
            printf("%s [%ld] (parent : %s)\n", getStringOf(node->nodeName), node->leafInfo->value.num, node->nodeName != PROGRAM ? getStringOf(node->parent->nodeName) : "ROOT");
            break;

        case TK_ID:
            // printf("%d\n", node->dataType);
            printf("%s [id = %s] (parent : %s)\n",
                getStringOf(node->nodeName), 
                node->leafInfo->value.idPtr,
                getStringOf(node->parent->nodeName));
        break;

        case TK_RUID:
        case TK_FIELDID:
        case TK_FUNID:
            printf("%s [%s] (parent : %s)\n", getStringOf(node->nodeName), node->leafInfo->value.idPtr, getStringOf(node->parent->nodeName));
        break;

        default:
            printf("%s (parent : %s)\n", getStringOf(node->nodeName), node->nodeName != PROGRAM ? getStringOf(node->parent->nodeName) : "ROOT");
    }
}

int
verify(parseTreeNode* ptn, termType T) {
    // printf("(%d)", T);
    // printf("verifying %s\n", getStringOf(T));
    
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
    if (iter != NULL) for (; *iter != NULL; iter = &((*iter)->rightSibling));
        
    *iter = (astNode*) malloc(sizeof(astNode));
    (*iter)->nodeName = T;
    (*iter)->depth = depth;
    (*iter)->parent = *ast;

    return iter;
}

void
addId(parseTreeNode *ptn, astNode** ast, int depth, termType datatype, char* ruid, int isGlobal) {
    astNode** iter = &(*ast)->leftChild;
    if (iter != NULL) for (; *iter != NULL; iter = &((*iter)->rightSibling));
        
    *iter = (astNode*) malloc(sizeof(astNode));
    (*iter)->nodeName = TK_ID;
    (*iter)->depth = depth;
    (*iter)->parent = *ast;

    (*iter)->leafInfo = (astLeafInfo*)malloc(sizeof(astLeafInfo));
    (*iter)->leafInfo->value.idPtr = strdup(ptn->tokenInfo.tokenPtr->value.idPtr);
    
    // printf("%d ==> %s\n", datatype, ruid);

    if (datatype == TK_INT || datatype == TK_REAL) {
        (*iter)->leafInfo->dataType.ttype = datatype;
        (*iter)->leafInfo->simple = 1;
        (*iter)->leafInfo->isGlobal = isGlobal;
    } else if (datatype == TK_RECORD || datatype == TK_UNION || ruid != NULL) {
        (*iter)->leafInfo->dataType.rtype = ruid;
        (*iter)->leafInfo->isGlobal = isGlobal;
        (*iter)->leafInfo->simple = 0;
    } else {
        (*iter)->leafInfo->isGlobal = -1;
        (*iter)->leafInfo->simple = -1;
    }
}

void
addFieldId(parseTreeNode *ptn, astNode** ast, int depth, termType type, char* ruid) {
    astNode** iter = &(*ast)->leftChild;
    if (iter != NULL) for (; *iter != NULL; iter = &((*iter)->rightSibling));
        
    *iter = (astNode*) malloc(sizeof(astNode));
    (*iter)->leafInfo = (astLeafInfo*)malloc(sizeof(astLeafInfo));
    (*iter)->nodeName = TK_FIELDID;
    (*iter)->depth = depth;
    (*iter)->parent = *ast;
    (*iter)->leafInfo->value.idPtr = strdup(ptn->tokenInfo.tokenPtr->value.idPtr);

    if (ruid == NULL && type == -1) return;

    if (ruid == NULL) {
        (*iter)->leafInfo->dataType.ttype = type;
        (*iter)->leafInfo->simple = 1;
    }
    else {
        (*iter)->leafInfo->dataType.rtype = ruid;
        (*iter)->leafInfo->simple = 0;
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

    // termType type;
    // char* ruid = NULL;

    // if(! ptn->) {
    //     type = ptn->leftChild->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type;
    // } else {
    //     ruid = strdup(ptn->leftChild->rightSibling->leftChild->tokenInfo.tokenPtr->value.idPtr);
    // }

    addId(ptn->leftChild, ast, depth, -1, NULL, -1);
    if (ptn->leftChild->rightSibling != NULL) {
        addMoreIds(ptn->leftChild->rightSibling, ast, depth);
    }
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
    astNode** iter = addNode(ast, DEFINETYPESTMT, depth);
    addRuid(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
    addRuid(ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling, iter, depth + 1);
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
    if (iter != NULL) for (; *iter != NULL; iter = &((*iter)->rightSibling));
        
    *iter = (astNode*) malloc(sizeof(astNode));
    (*iter)->leafInfo = (astLeafInfo*)malloc(sizeof(astLeafInfo));
    (*iter)->nodeName = TK_RUID;
    (*iter)->depth = depth;
    (*iter)->parent = *ast;
    (*iter)->leafInfo->value.idPtr = strdup(ptn->tokenInfo.tokenPtr->value.idPtr);
}

void
addFunId(parseTreeNode *ptn, astNode** ast, int depth) {
    astNode** iter = &(*ast)->leftChild;
    if (iter != NULL) for (; *iter != NULL; iter = &((*iter)->rightSibling));
        
    *iter = (astNode*) malloc(sizeof(astNode));
    (*iter)->leafInfo = (astLeafInfo*)malloc(sizeof(astLeafInfo));
    (*iter)->nodeName = TK_FUNID;
    (*iter)->depth = depth;
    (*iter)->parent = *ast;
    (*iter)->leafInfo->value.idPtr = strdup(ptn->tokenInfo.tokenPtr->value.idPtr);
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
    termType type;
    char* ruid = NULL;
    if(! ptn->leftChild->rightSibling->leftChild->isTerminal) {
        type = ptn->leftChild->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type;
    } else {
        ruid = strdup(ptn->leftChild->rightSibling->leftChild->tokenInfo.tokenPtr->value.idPtr);
    }
    addFieldId(ptn->leftChild->rightSibling->rightSibling->rightSibling, ast, depth, type, ruid);
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
                addTypeDefs(temp, iter, depth, 0);
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

    termType type = ptn->leftChild->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type;
    char* ruid = NULL;
    if (type == TK_RECORD || type == TK_UNION) {
        ruid = ptn->leftChild->rightSibling->leftChild->leftChild->rightSibling->tokenInfo.tokenPtr->value.idPtr;
    } else if (type == TK_RUID) {
        ruid = ptn->leftChild->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->value.idPtr;
    }

    addId(temp, ast, depth, type, ruid, !(temp->rightSibling->isTerminal));
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
        addDecls(ptn->leftChild->rightSibling, iter, depth, 0);

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
    addFieldId(ptn->leftChild->rightSibling, ast, depth, -1, NULL);
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

    if (ptn->leftChild->rightSibling != NULL) {
        iter = addNode(ast, SINGLEORRECID, depth);
        depth++;
    }
    else iter = ast;

    addId(ptn->leftChild, iter, depth, -1, NULL, -1);
    
    if (iter != ast) {
        addConstVar(ptn->leftChild->rightSibling, iter, depth);
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
    astNode** iter = addNode(ast, TK_RNUM, depth);
    (*iter)->leafInfo = (astLeafInfo*)malloc(sizeof(astLeafInfo));
    // printf("%s===", ptn->tokenInfo.tokenPtr->value.idPtr);
    sscanf(ptn->tokenInfo.tokenPtr->value.idPtr, "%lf", &((*iter)->leafInfo->value.rnum));
    // printf("%lf\n", (*iter)->value.rnum);
}

void
addNum(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, TK_NUM);
    astNode** iter = addNode(ast, TK_NUM, depth);
    (*iter)->leafInfo = (astLeafInfo*)malloc(sizeof(astLeafInfo));
    sscanf(ptn->tokenInfo.tokenPtr->value.idPtr, "%ld", &((*iter)->leafInfo->value.num));
    // (*iter)->value.num = atoi(ptn->tokenInfo.tokenPtr->value.idPtr);
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
        addArithExp(ptn->leftChild->rightSibling, ast, depth); 
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
                ptn->leftChild->rightSibling->leftChild->leftChild->tokenInfo.tokenPtr->type,
                depth);
    } else iter = ast;

    if (iter != ast) depth ++;

    addTerm(ptn->leftChild, iter, depth);
    
    if (iter != ast)
        addExpPrime(ptn->leftChild->rightSibling, iter, depth);
}

void
addOutputPars(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, OUTPUTPARAMETERS);
    astNode** iter = addNode(ast, OUTPUTPARAMETERS, depth);
    addIdList(ptn->leftChild->rightSibling, iter, depth + 1);
}

void
addInputPars(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, INPUTPARAMETERS);
    astNode** iter = addNode(ast, INPUTPARAMETERS, depth);
    addIdList(ptn->leftChild->rightSibling, iter, depth + 1);
}

void
addFnCallStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, FUNCALLSTMT);
    astNode** iter = addNode(ast, FUNCALLSTMT, depth);
    addFunId(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);

    if (! ptn->leftChild->isTerminal) {
        addOutputPars(ptn->leftChild, iter, depth + 1);
    }
    addInputPars(ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, iter, depth + 1);
}

void
addBoolExp(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, BOOLEANEXPRESSION);
    astNode** iter;
    if (ptn->leftChild->isTerminal) {
        if (ptn->leftChild->tokenInfo.tokenPtr->type == TK_NOT) {
            iter = addNode(ast, TK_NOT, depth);
            addBoolExp(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
        } else {
            iter = addNode(
                ast,
                ptn->leftChild->rightSibling->rightSibling->rightSibling->leftChild->tokenInfo.tokenPtr->type,
                depth);

            addBoolExp(ptn->leftChild->rightSibling, iter, depth + 1);

            addBoolExp(
                ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling,
                iter,
                depth + 1);
        }
    } else {
        iter = addNode(ast, ptn->leftChild->rightSibling->leftChild->tokenInfo.tokenPtr->type, depth);
        addVar(ptn->leftChild, iter, depth + 1);
        addVar(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
    }
}

void
addIterStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ITERATIVESTMT);
    astNode** iter = addNode(ast, ITERATIVESTMT, depth);
    addBoolExp(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
    addStmt(ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling, iter, depth + 1);
    if (! ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->isTerminal)
        addOtherStmts(ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling, iter, depth + 1);
}

int
addThenPart(parseTreeNode *ptn, astNode** ast, int depth) {
    astNode** iter = addNode(ast, TK_THEN, depth);
    addStmt(ptn, iter, depth + 1);
    if (ptn->rightSibling->tokenInfo.tokenType != ELSEPART) {
        addOtherStmts(ptn->rightSibling, iter, depth + 1);
        return 1; // indicates that there are other statements before else
    }
    return 0; // indicates that there is only one statement in the then part
}

void
addElsePart(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ELSEPART);

    if (ptn->leftChild->rightSibling == NULL) return;
    astNode** iter = addNode(ast, TK_ELSE, depth);
    addStmt(ptn->leftChild->rightSibling, iter, depth + 1);
    if (!ptn->leftChild->rightSibling->rightSibling->isTerminal)
        addOtherStmts(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
}

void
addCondStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, CONDITIONALSTMT);
    astNode** iter = addNode(ast, CONDITIONALSTMT, depth);
    addBoolExp(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);

    int moreThanOne = addThenPart(
        ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling,
        iter,
        depth + 1);

    if (moreThanOne)
        addElsePart(
            ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling,
            iter,
            depth + 1);
    else
        addElsePart(
            ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling->rightSibling,
            iter,
            depth + 1);
}

void
addIOStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, IOSTMT);
    astNode** iter;
    if (ptn->leftChild->tokenInfo.tokenPtr->type == TK_READ) iter = addNode(ast, TK_READ, depth);
    else iter = addNode(ast, TK_WRITE, depth);

    addVar(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
}

void
addAssignStmt(parseTreeNode *ptn, astNode** ast, int depth) {
    verify(ptn, ASSIGNMENTSTMT);
    astNode** iter = addNode(ast, ASSIGNMENTSTMT, depth);
    addSingOrRecId(ptn->leftChild, iter, depth + 1);
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
                addIterStmt(temp, ast, depth);
                break;

            case CONDITIONALSTMT:
                addCondStmt(temp, ast, depth);
                break;

            case IOSTMT:
                addIOStmt(temp, ast, depth);
                break;

            case FUNCALLSTMT:    
                addFnCallStmt(temp, ast, depth);
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

    termType type;
    char* ruid = NULL;
    if(ptn->leftChild->leftChild->leftChild->tokenInfo.tokenPtr->type == TK_INT ||
        ptn->leftChild->leftChild->leftChild->tokenInfo.tokenPtr->type == TK_REAL) {
        type = ptn->leftChild->leftChild->leftChild->tokenInfo.tokenPtr->type;
    } else if  (ptn->leftChild->leftChild->leftChild->tokenInfo.tokenPtr->type == TK_RECORD ||
        ptn->leftChild->leftChild->leftChild->tokenInfo.tokenPtr->type == TK_UNION) {
        type = ptn->leftChild->leftChild->leftChild->tokenInfo.tokenPtr->type;
        ruid = strdup(ptn->leftChild->leftChild->leftChild->rightSibling->tokenInfo.tokenPtr->value.idPtr);
    } else {
        ruid = strdup(ptn->leftChild->leftChild->leftChild->tokenInfo.tokenPtr->value.idPtr);
    }

    addId(ptn->leftChild->rightSibling, iter, depth + 1, type, ruid, 1);
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
    if (! ptn->leftChild->rightSibling->rightSibling->isTerminal) {
        addOutPar(ptn->leftChild->rightSibling->rightSibling, iter, depth + 1);
        addStmts(ptn->leftChild->rightSibling->rightSibling->rightSibling->rightSibling, iter, depth + 1);
    } else {
        addStmts(ptn->leftChild->rightSibling->rightSibling->rightSibling, iter, depth + 1);
    }
}

void
addOtherFn(parseTreeNode *ptn, astNode** ast, int depth, int createNode) {
    verify(ptn, OTHERFUNCTION);

    astNode** iter;
    if (createNode) iter = addNode(ast, OTHERFUNCTION, depth);
    else iter = ast;

    addFunction(ptn->leftChild, iter, depth + 1);
    if(ptn->leftChild->rightSibling != NULL)
        addOtherFn(ptn->leftChild->rightSibling, iter, depth, 0);
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


// long aSize;
// int aNodes;
// returnTup getAstNodeSize(astNode* ast) {
//     aSize = 0;
//     aNodes = 0;

//     printf("%d %d\n", sizeof(astNode) + sizeof(astLeafInfo), sizeof(astNode));

//     returnTup r;
//     getAstSize(ast);
//     r.nodes = aNodes;
//     r.size = aSize;
//     return r;
// }

// void getAstSize(astNode* node) {
//     astNode* sib = node;

//     switch(node->nodeName) {
//         case TK_ID: case TK_FIELDID: case TK_RUID: case TK_FUNID:
//         case TK_RNUM: case TK_NUM:
//             aSize += sizeof(astNode) + sizeof(astLeafInfo);
//             printf("counted terminal with info (%s)\n", getStringOf(node->nodeName));
//             flg = 1;
//             // sib = sib->rightSibling;
//             break;
        
//         default:
//             aSize += sizeof(astNode);
//             printf("counted terminal/nonterminal without info (%s)\n", getStringOf(node->nodeName));
//             // sib = sib->rightSibling;
//             break;
//     }
    
//     aNodes += 1;

//     for (int j = 0; sib != NULL; j++, sib = sib->rightSibling) {
//         if (j > 0) getAstSize(sib);
//         if (flg == 1 || sib->leftChild == NULL) return;
//         getAstSize(sib->leftChild);
//     }
//     return;
// }

long getSize(termType t) {
    switch(t) {
        case TK_ID: case TK_FIELDID: case TK_RUID: case TK_FUNID: case TK_RNUM: case TK_NUM:
            return sizeof(astNode) + sizeof(astLeafInfo);
        default:
            return sizeof(astNode);
    }
}

void getAstSize(astNode* node, long* nodes, long* size) {
    if(node->leftChild != NULL) {
        getAstSize(node->leftChild, nodes, size);

        (*nodes) += 1;
        // printf("printing %ld : %ld, %ld\n",*nodes, *size, getSize(node->nodeName));
        (*size) = (*size) + getSize(node->nodeName);

        node = node->leftChild->rightSibling;
        while (node != NULL) {
            getAstSize(node, nodes, size);
            node = node->rightSibling;
        }
        return;
    }

    (*nodes) += 1;
    (*size) += getSize(node->nodeName);
}
