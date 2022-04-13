#include "semantic.h"
#include <string.h>
#include <stdlib.h>

void makeConTypeTable(astNode* ast, conTypeWrapper* cwrap) {
   // assuming that we get the "root" / "program" node as input to this function
   astNode* iter = ast->leftChild;
   for(; iter != NULL; iter = iter->rightSibling) {
      if (iter->nodeName == MAINFUNCTION) {
         if (iter->leftChild->leftChild->nodeName == TYPEDEFINITIONS) {
            astNode* sub = iter->leftChild->leftChild->leftChild;
            // printf("sub %s\n", getStringOf(sub->nodeName));
            for(; sub != NULL; sub = sub->rightSibling) {
               // printf("got %s %s\n", getStringOf(sub->leftChild->nodeName), sub->leftChild->leafInfo->value.idPtr);
               (*cwrap).ctypes[cwrap->length].ruid = sub->leftChild->leafInfo->value.idPtr;

               (*cwrap).ctypes[cwrap->length].typexp = (contypexp*)malloc(sizeof(contypexp));
               contypexp* texp = (*cwrap).ctypes[cwrap->length].typexp;
               int flg = 0;
               for (astNode* fds = sub->leftChild->rightSibling->leftChild; fds != NULL; fds = fds->rightSibling) {
                  if (flg) {
                     texp->next = (contypexp*)malloc(sizeof(contypexp));
                     texp = texp->next;
                     // printf("came here\n");
                  }
                  if (flg == 0) flg = 1;
                  if (fds->leafInfo->simple) {
                     // printf("qweqwe %s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, getStringOf(fds->leafInfo->dataType.ttype));
                     texp->type.ttype = fds->leafInfo->dataType.ttype;
                     (*cwrap).ctypes[cwrap->length].typewidth += texp->type.ttype == TK_INT ? 2 : 4;
                  }
                  else {
                     // printf("qweqwe %s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, fds->leafInfo->dataType.rtype);
                     texp->type.rtype = fds->leafInfo->dataType.rtype;
                     // LOOK AT WHAT TO DO IN THIS CASE
                  }
               }
               cwrap->length++;
            }
         }
      } else {
         astNode *sub, *liter;
         liter = iter->leftChild;

         for (; liter != NULL; liter = liter->rightSibling) {
            if (liter->leftChild->rightSibling->rightSibling->leftChild->nodeName == TYPEDEFINITIONS) {
               sub = liter->leftChild->rightSibling->rightSibling->leftChild->leftChild;
            } else if (liter->leftChild->rightSibling->rightSibling->rightSibling->leftChild->nodeName == TYPEDEFINITIONS) {
               sub = liter->leftChild->rightSibling->rightSibling->rightSibling->leftChild->leftChild;
            }
            for(; sub != NULL; sub = sub->rightSibling) {
               // printf("got %s %s\n", getStringOf(sub->leftChild->nodeName), sub->leftChild->leafInfo->value.idPtr);
               (*cwrap).ctypes[cwrap->length].ruid = sub->leftChild->leafInfo->value.idPtr;
               
               // LOOKOUT FOR COPY_PASTA ERRORS !!!
               (*cwrap).ctypes[cwrap->length].typexp = (contypexp*)malloc(sizeof(contypexp));
               contypexp* texp = (*cwrap).ctypes[cwrap->length].typexp;
               int flg = 0;
               for (astNode* fds = sub->leftChild->rightSibling->leftChild; fds != NULL; fds = fds->rightSibling) {
                  if (flg) {
                     texp->next = (contypexp*)malloc(sizeof(contypexp));
                     texp = texp->next;
                     // printf("came here\n");
                  }
                  if (flg == 0) flg = 1;
                  if (fds->leafInfo->simple) {
                     // printf("qweqwe %s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, getStringOf(fds->leafInfo->dataType.ttype));
                     texp->type.ttype = fds->leafInfo->dataType.ttype;
                     (*cwrap).ctypes[cwrap->length].typewidth += texp->type.ttype == TK_INT ? 2 : 4;
                  }
                  else {
                     // printf("qweqwe %s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, fds->leafInfo->dataType.rtype);
                     texp->type.rtype = fds->leafInfo->dataType.rtype;
                     // LOOK AT WHAT TO DO IN THIS CASE
                  }
               }

               cwrap->length++;
            }
         }
      }
   }
}

void printConTypeTable(conTypeWrapper* cwrap) {
   printf("RECORD AND UNION TABLE\n");
   for (int i = 0; i < cwrap->length; i++) {
      printf("%s -> <", (*cwrap).ctypes[i].ruid);
      contypexp* texp = (*cwrap).ctypes[i].typexp;
      while (texp != NULL) {
         printf("%s", (texp->type.ttype >= 0 && texp->type.ttype < TERMTYPESIZE) ? getStringOf(texp->type.ttype) : texp->type.rtype);
         texp = texp->next;
         if (texp != NULL) printf(", ");
         else printf(">");
      }
      printf(" [%ld]\n", (*cwrap).ctypes[i].typewidth);  
   }
}

void makeSymbolTables(astNode* ast) {
   astNode* iter = ast->leftChild;
   for(; iter!=NULL; iter->rightSibling) {
      if(iter->nodeName == OTHERFUNCTION) {
         makeSymbolTable(iter->leftChild->leftChild); //calling on funid
      }
   
      else if(iter->nodeName == MAINFUNCTION) {
         makeSymbolTable(iter); //calling on main
      }
   }
}

symbolTable* makeSymbolTable(astNode* ast) {
   //initialize symboltable


   if(ast->nodeName == MAINFUNCTION) {
      //set scopename to main

      astNode* sib = ast->leftChild->leftChild;
      if(sib->nodeName == TYPEDEFINITIONS) sib = sib->rightSibling; //sib is now at declarations
      if(sib->nodeName == DECLARATIONS) {
         //printf("HERE : %s\n", getStringOf(sib->nodeName));
         sib = sib->leftChild; //sib is now at first declaration
         for(; sib!=NULL; sib = sib->rightSibling) { //iterating over declarations
            //make symboltable entries and add
         }
      } else return NULL;
   }

   else if(ast->nodeName == TK_FUNID) {
      //set scopename to funid
      //printf("FUNC NAME : %s\n", ast->leafInfo->value.idPtr);
      astNode* sib = ast->rightSibling->rightSibling;
      if(sib->nodeName == PARAMETER_LIST) sib = sib->rightSibling; //sib is now at stmts
      //printf("HERE : %s\n", getStringOf(sib->nodeName));
      sib = sib->leftChild;
      if(sib->nodeName == TYPEDEFINITIONS) sib = sib->rightSibling; //sib may now be at declarations (there may be no declarations)
      if(sib->nodeName == DECLARATIONS) {
         //printf("HERE : %s\n", getStringOf(sib->nodeName));
         sib = sib->leftChild; //sib is now at first declaration
         for(; sib!=NULL; sib = sib->rightSibling) { //iterating over declarations
            //make symboltable entries and add
         }
      } else return NULL;
   }
}



void printSymbolTable(symbolTable* table) {

}

// symbolTable* makeTable(astNode* ast, symbolTable* parentTable) {
//    // assuming that we get the "root" / "program" node as input to this function

// }