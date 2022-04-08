#include "semantic.h"
#include <stdlib.h>

void printAstNode(astNode* node) {
   if(node==NULL) printf("null\n");
   printf("Node Type : %s\n", getStringOf(node->nodeName));
}

void makeNode(parseTreeNode *ptn, astNode** ast, int depth) {
   printf("1\n");

   parseTreeNode* check = ptn->leftChild;
   if(check != NULL) {
      (**ast).leftChild = (astNode*)malloc(sizeof(astNode));
      (*ast)->leftChild = mkAST(check, depth);
      if ((**ast).leftChild != NULL) {
         (*ast)->leftChild->parent = (*ast);
         check = ptn->leftChild->rightSibling;
      } else check = NULL;
   }
   astNode* next = (*ast)->leftChild;
   while(check != NULL) {
      // printAstNode(next);
      astNode* temp;
      temp = (astNode*)malloc(sizeof(astNode));
      temp = mkAST(check, depth);
      //if(temp == NULL) printf("temp is null\n");
      if (temp != NULL) {
         next->rightSibling = temp;
         next->rightSibling->parent = next->parent;
         check = check->rightSibling;
         next = next->rightSibling;
      } else break;
   }
}

void printAST(astNode* node) {
   astNode* sib = node;

   for (int j = 0; sib != NULL; j ++, sib = sib->rightSibling) {
      for(int i=0; i<sib->depth; i++) printf("---");
      printAstNode(sib);
      if (sib->leftChild != NULL) {
         // printf("\t");
         printAST(sib->leftChild);
      }
      // printf("j = %d\n", j); 
   }
}

void makeNodes(parseTreeNode *ptn, int depth, astNode** node) {
   if (ptn == NULL) return;

   printf("making nodes -> ");

   *node = (astNode*)malloc(sizeof(astNode));

   if(ptn->isTerminal) {
      printf("\n");
      switch (ptn->tokenInfo.tokenPtr->type)
      {
      case TK_MAIN:
         return;
      
      default:
         break;
      }
   } else {
      printf("nonterminal ==> ");
      switch (ptn->tokenInfo.tokenType)
      {
         case PROGRAM:
            printf("PROGRAM\n");
            (*node)->nodeName = PROGRAM;
            printAstNode(*node);
            break;
            
         case MAINFUNCTION:
            printf("MAIN\n");
            (*node)->nodeName = MAINFUNCTION;
            break;
         
         case STMTS:
            printf("STMTS\n");
            (*node)->nodeName = STMTS;
            break;
      
      default:
         printf("unimplemented\n");
         break;
      }
   }

   for (parseTreeNode* iter = ptn->leftChild; iter != NULL; iter = iter->rightSibling) {
      makeNodes(iter, depth+1, &((*node)->leftChild));
   }
}

astNode* makeAST(parseTreeNode *ptn, int depth) {
   astNode *ast;
   // ast = (astNode*)malloc(sizeof(astNode));
   // ast->nodeName = PROGRAM;
   // ast->depth = depth;

   makeNodes(ptn, depth, &ast); 

   printAstNode(ast);
   printAstNode(ast->leftChild);
   printAstNode(ast->leftChild->leftChild);
   printAstNode(ast->leftChild->rightSibling);
   return ast;  
}

astNode* mkAST(parseTreeNode *ptn, int depth) {
   if (ptn == NULL) {
      return NULL;
   }

   astNode *ast;
   astNode* temp;

   if (ptn->isTerminal) {
      switch(ptn->tokenInfo.tokenPtr->type) {
         case TK_MAIN:
         case TK_RETURN:
         case TK_SEM:
         case TK_SQL:
         case TK_SQR:
         case TK_COMMA:
            return mkAST(ptn->rightSibling, depth);

         case TK_ID: printf("ID!!!\n");
            ast = (astNode*)malloc(sizeof(astNode));
            ast->nodeName = TK_ID;
            ast->stEntry = NULL;
            ast->depth = depth;
            return ast;
      }
   } else {
      printf("generating %s\n", getStringOf(ptn->tokenInfo.tokenType));
      switch (ptn->tokenInfo.tokenType) {
         case PROGRAM:
            ast = (astNode*)malloc(sizeof(astNode));
            ast->nodeName = PROGRAM;
            ast->parent = NULL;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return ast;
            break;

         case MAINFUNCTION:
            printf("inside main\n");
            return mkAST(ptn->leftChild, depth);

         case OTHERFUNCTION:
            printf("solving this for now\n");
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = OTHERFUNCTION;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;
         
         case FUNCTION:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = FUNCTION;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;
            
         case INPUT_PAR:
            return mkAST(ptn->leftChild, depth);

         case OUTPUT_PAR:
            return mkAST(ptn->leftChild, depth);
         
         case STMTS:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = STMTS;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;
         
         case RETURNSTMT:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = RETURNSTMT;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;
            
         case OPTIONALRETURN:
            return mkAST(ptn->leftChild, depth);
            
         case IDLIST:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = IDLIST;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;

         case MORE_IDS:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = MORE_IDS;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;

         case PARAMETER_LIST:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = PARAMETER_LIST;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;

         case REMAINING_LIST:
            return mkAST(ptn->leftChild, depth);

         case TYPEDEFINITIONS:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = TYPEDEFINITIONS;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;
         
         case ACTUALORREDEFINED:
            return mkAST(ptn->leftChild, depth);

         case TYPEDEFINITION:
            ast = (astNode*)malloc(sizeof(astNode));
            temp = ast;
            ast->nodeName = TYPEDEFINITION;
            ast->stEntry = NULL;
            ast->depth = depth;
            makeNode(ptn, &ast, depth + 1);
            return temp;
         
         default: printf("%s not implemented yet\n", getStringOf(ptn->tokenInfo.tokenType));
      }
   }  
}