// Group number 13
// Ashwin Kiran Godbole 2018B5A70423P
// Samarth Krishna Murthy 2018B2A70362P

#include "lexer.h"
#include "lexerDef.h"
#include "semantic.h"
#include "semanticDef.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void makeConTypeTable(astNode *ast, conTypeWrapper *cwrap)
{
   // assuming that we get the "root" / "program" node as input to this function
   astNode *iter = ast->leftChild;
   for (; iter != NULL; iter = iter->rightSibling)
   {
      if (iter->nodeName == MAINFUNCTION)
      {
         if (iter->leftChild->leftChild->nodeName == TYPEDEFINITIONS)
         {
            astNode *sub = iter->leftChild->leftChild->leftChild;

            for (; sub != NULL; sub = sub->rightSibling)
            {
               if (sub->nodeName == DEFINETYPESTMT)
               {
                  for (int i = 0; i < cwrap->length; i++)
                  {
                     if (!strcmp(
                             sub->leftChild->leafInfo->value.idPtr,
                             cwrap->ctypes[i].ruid))
                     {
                        cwrap->ctypes[i].aliases[cwrap->ctypes[i].numalias++] = strdup(sub->leftChild->rightSibling->leafInfo->value.idPtr);
                        break;
                     }
                  }
                  continue;
               }

               (*cwrap).ctypes[cwrap->length].ruid = sub->leftChild->leafInfo->value.idPtr;

               (*cwrap).ctypes[cwrap->length].typexp = (contypexp *)malloc(sizeof(contypexp));
               contypexp *texp = (*cwrap).ctypes[cwrap->length].typexp;
               int flg = 0;
               for (astNode *fds = sub->leftChild->rightSibling->leftChild; fds != NULL; fds = fds->rightSibling)
               {
                  // printf("for fielddef %s\n", fds->leafInfo->value.idPtr);
                  if (flg)
                  {
                     texp->next = (contypexp *)malloc(sizeof(contypexp));
                     texp = texp->next;
                     // printf("came here\n");
                  }
                  if (flg == 0)
                     flg = 1;
                  if (fds->leafInfo->simple == 1)
                  {
                     // printf("%s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, getStringOf(fds->leafInfo->dataType.ttype));
                     texp->type.ttype = fds->leafInfo->dataType.ttype;
                     (*cwrap).ctypes[cwrap->length].typewidth += texp->type.ttype == TK_INT ? 2 : 4;
                  }
                  else
                  {
                     // printf("%s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, fds->leafInfo->dataType.rtype);
                     texp->type.rtype = fds->leafInfo->dataType.rtype;
                     // printf("length : %d\n", cwrap->length);
                     for (int i = 0; i < cwrap->length; i++)
                     {
                        // printf("%s\n", cwrap->ctypes[i].ruid);
                        if (!strcmp(texp->type.rtype, cwrap->ctypes[i].ruid))
                        {
                           (*cwrap).ctypes[cwrap->length].typewidth += cwrap->ctypes[i].typewidth;
                        }
                        int l = 0;
                        for (int j = 0; j < cwrap->ctypes[i].numalias; j++)
                        {
                           if (!strcmp(texp->type.rtype, cwrap->ctypes[i].aliases[j]))
                           {
                              (*cwrap).ctypes[cwrap->length].typewidth += cwrap->ctypes[i].typewidth;
                              l = 1;
                              break;
                           }
                        }
                        if (l == 1)
                           break;
                     }
                  }
               }
               cwrap->length++;
            }
         }
      }
      else
      {
         astNode *sub, *liter;
         liter = iter->leftChild;

         for (; liter != NULL; liter = liter->rightSibling)
         {
            if (liter->leftChild->rightSibling->rightSibling->leftChild->nodeName == TYPEDEFINITIONS)
            {
               sub = liter->leftChild->rightSibling->rightSibling->leftChild->leftChild;
            }
            else if (liter->leftChild->rightSibling->rightSibling->rightSibling->leftChild->nodeName == TYPEDEFINITIONS)
            {
               sub = liter->leftChild->rightSibling->rightSibling->rightSibling->leftChild->leftChild;
            }
            for (; sub != NULL; sub = sub->rightSibling)
            {
               if (sub->nodeName == DEFINETYPESTMT)
               {
                  for (int i = 0; i < cwrap->length; i++)
                  {
                     if (!strcmp(
                             sub->leftChild->leafInfo->value.idPtr,
                             cwrap->ctypes[i].ruid))
                     {
                        cwrap->ctypes[i].aliases[cwrap->ctypes[i].numalias++] = strdup(sub->leftChild->rightSibling->leafInfo->value.idPtr);
                        break;
                     }
                  }
                  continue;
               }
               // printf("got %s %s\n", getStringOf(sub->leftChild->nodeName), sub->leftChild->leafInfo->value.idPtr);
               (*cwrap).ctypes[cwrap->length].ruid = sub->leftChild->leafInfo->value.idPtr;

               (*cwrap).ctypes[cwrap->length].typexp = (contypexp *)malloc(sizeof(contypexp));
               contypexp *texp = (*cwrap).ctypes[cwrap->length].typexp;
               int flg = 0;
               for (astNode *fds = sub->leftChild->rightSibling->leftChild; fds != NULL; fds = fds->rightSibling)
               {
                  if (flg)
                  {
                     texp->next = (contypexp *)malloc(sizeof(contypexp));
                     texp = texp->next;
                  }
                  if (flg == 0)
                     flg = 1;
                  if (fds->leafInfo->simple == 1)
                  {
                     // printf("%s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, getStringOf(fds->leafInfo->dataType.ttype));
                     texp->type.ttype = fds->leafInfo->dataType.ttype;
                     (*cwrap).ctypes[cwrap->length].typewidth += texp->type.ttype == TK_INT ? 2 : 4;
                  }
                  else
                  {
                     // printf("%s, %s, %s\n", getStringOf(fds->nodeName), fds->leafInfo->value.idPtr, fds->leafInfo->dataType.rtype);
                     texp->type.rtype = fds->leafInfo->dataType.rtype;
                     // printf("length : %d\n", cwrap->length);
                     for (int i = 0; i < cwrap->length; i++)
                     {
                        // printf(" %s\n", cwrap->ctypes[i].ruid);
                        if (!strcmp(texp->type.rtype, cwrap->ctypes[i].ruid))
                        {
                           (*cwrap).ctypes[cwrap->length].typewidth += cwrap->ctypes[i].typewidth;
                           break;
                        }
                        int l = 0;
                        for (int j = 0; j < cwrap->ctypes[i].numalias; j++)
                        {
                           if (!strcmp(texp->type.rtype, cwrap->ctypes[i].aliases[j]))
                           {
                              (*cwrap).ctypes[cwrap->length].typewidth += cwrap->ctypes[i].typewidth;
                              l = 1;
                              break;
                           }
                        }
                        if (l == 1)
                           break;
                     }
                  }
               }
               cwrap->length++;
            }
         }
      }
   }
}

void printConTypeTable(conTypeWrapper *cwrap)
{
   printf("RECORD AND UNION TABLE\n");
   for (int i = 0; i < cwrap->length; i++)
   {
      printf("%s <", (*cwrap).ctypes[i].ruid);
      contypexp *texp = (*cwrap).ctypes[i].typexp;
      while (texp != NULL)
      {
         printf("%s", (texp->type.ttype >= 0 && texp->type.ttype < TERMTYPESIZE) ? getStringOf(texp->type.ttype) : texp->type.rtype);
         texp = texp->next;
         if (texp != NULL)
            printf(", ");
         else
            printf(">");
      }
      printf(" [%ld]", (*cwrap).ctypes[i].typewidth);
      printf(" aliases = [");
      for (int j = 0; j < 5 && cwrap->ctypes[i].aliases[j] != NULL; j++)
      {
         printf("%s", cwrap->ctypes[i].aliases[j]);
         if (cwrap->ctypes[i].aliases[j + 1] != NULL)
            printf(", ");
      }
      printf("]\n");
   }
}

void makeSymbolTables(astNode *ast, symbolTable *globalTable, symbolTable *localTables, conTypeWrapper *cwrap)
{
   // assuming that this is called on "program" node only
   astNode *iter = ast->leftChild;
   for (int i = 0; iter != NULL; iter = iter->rightSibling, i++)
   {
      if (iter->nodeName == OTHERFUNCTION)
      {
         // put a for loop to include all the functions under otherfunction !
         astNode *sib = iter->leftChild;
         for (int j = 1; sib != NULL; sib = sib->rightSibling, j++)
         {

            makeSymbolTable(sib->leftChild, globalTable, localTables, j, cwrap); // calling on funid
         }
      }
      else if (iter->nodeName == MAINFUNCTION)
      {
         makeSymbolTable(iter, globalTable, localTables, 0, cwrap); // calling on main
      }
   }
}

void makeSymbolTable(astNode *ast, symbolTable *globalTable, symbolTable *localTables, int index, conTypeWrapper *cwrap)
{
   // initialize symboltable
   if (ast->nodeName == MAINFUNCTION)
   {
      // set scopename to main
      astNode *sib = ast->leftChild->leftChild;
      if (sib->nodeName == TYPEDEFINITIONS)
         sib = sib->rightSibling; // sib is now at declarations
      if (sib->nodeName == DECLARATIONS)
      {
         // printf("HERE : %s\n", getStringOf(sib->nodeName));
         sib = sib->leftChild; // sib is now at first declaration
         for (; sib != NULL; sib = sib->rightSibling)
         { // iterating over declarations
            symbolTable *table;

            if (sib->leafInfo->isGlobal)
            {
               table = globalTable;
               table->scopeName = strdup("global");
            }
            else
            {
               table = &(localTables[index]);
               table->scopeName = strdup("main");
            }

            symbolTableEntry *entry;
            if (table->symbolTableEntries == NULL)
            {
               table->symbolTableEntries = (symbolTableEntry *)malloc(sizeof(symbolTableEntry));
               entry = table->symbolTableEntries;
               entry->offset = 0;
            }
            else
            {
               entry = table->symbolTableEntries;
               int total = 0;
               while (entry->nextEntry != NULL)
               {
                  total += entry->offset;
                  entry = entry->nextEntry;
               }
               entry->nextEntry = (symbolTableEntry *)malloc(sizeof(symbolTableEntry));
               entry->nextEntry->offset = total + entry->width;
               entry = entry->nextEntry;
            }

            entry->name = sib->leafInfo->value.idPtr;

            if (table == globalTable)
               entry->scopNat = GLOBAL;
            else
               entry->scopNat = LOCAL;

            if (sib->leafInfo->simple == 1)
            {
               entry->typeExp.tag = sib->leafInfo->dataType.ttype;
               entry->width = entry->typeExp.tag == TK_INT ? 2 : 4;
            }
            else
            {
               entry->typeExp.ruid = sib->leafInfo->dataType.rtype;
               for (int i = 0; i < cwrap->length; i++)
               {
                  if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].ruid))
                  {
                     entry->width = cwrap->ctypes[i].typewidth;
                     break;
                  }
                  int l = 0;
                  for (int j = 0; j < cwrap->ctypes[i].numalias; j++)
                  {
                     if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].aliases[j]))
                     {
                        entry->width = cwrap->ctypes[i].typewidth;
                        l = 1;
                        break;
                     }
                  }
                  if (l == 1)
                     break;
               }
            }
         }
      }
   }

   else if (ast->nodeName == TK_FUNID)
   {
      int offafterin = 0;
      astNode *bib = ast->rightSibling->leftChild;
      symbolTable *table = &(localTables[index]);
      table->scopeName = strdup(ast->leafInfo->value.idPtr);
      for (; bib != NULL; bib = bib->rightSibling)
      {
         symbolTableEntry *entry;
         if (table->symbolTableEntries == NULL)
         {
            table->symbolTableEntries = (symbolTableEntry *)malloc(sizeof(symbolTableEntry));
            entry = table->symbolTableEntries;
            entry->offset = 0;
         }
         else
         {
            entry = table->symbolTableEntries;
            int total = 0;
            while (entry->nextEntry != NULL)
            {
               total += entry->offset;
               entry = entry->nextEntry;
            }
            //printf("TOTAL : %d\n",total);
            entry->nextEntry = (symbolTableEntry *)malloc(sizeof(symbolTableEntry));
            entry->nextEntry->offset = total + entry->width;
            entry = entry->nextEntry;
         }

         entry->name = bib->leafInfo->value.idPtr;
         entry->scopNat = LOCAL;
         entry->us = INPUTPAR;

         if (bib->leafInfo->simple == 1)
         {
            entry->typeExp.tag = bib->leafInfo->dataType.ttype;
            entry->width = entry->typeExp.tag == TK_INT ? 2 : 4;
            offafterin = entry->offset + entry->width;
         }
         else
         {
            entry->typeExp.ruid = bib->leafInfo->dataType.rtype;
            for (int i = 0; i < cwrap->length; i++)
            {
               if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].ruid))
               {
                  entry->width = cwrap->ctypes[i].typewidth;
                  offafterin = entry->offset + entry->width;
                  break;
               }
               int l = 0;
               for (int j = 0; j < cwrap->ctypes[i].numalias; j++)
               {
                  if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].aliases[j]))
                  {
                     entry->width = cwrap->ctypes[i].typewidth;
                     l = 1;
                     break;
                  }
               }
               if (l == 1)
                  break;
            }
         }
      }

      if (ast->rightSibling->rightSibling->nodeName == PARAMETER_LIST)
      {
         bib = ast->rightSibling->rightSibling->leftChild;
         // table = &(localTables[index]);
         table->scopeName = strdup(ast->leafInfo->value.idPtr);
         for (; bib != NULL; bib = bib->rightSibling)
         {
            symbolTableEntry *entry;

            entry = table->symbolTableEntries;
            int total = 0;
            while (entry->nextEntry != NULL)
            {
               // total += entry->offset;
               entry = entry->nextEntry;
            }
            entry->nextEntry = (symbolTableEntry *)malloc(sizeof(symbolTableEntry));
            entry->nextEntry->offset = offafterin;// total + entry->width;
            entry = entry->nextEntry;


            entry->name = bib->leafInfo->value.idPtr;
            entry->scopNat = LOCAL;
            entry->us = OUTPUTPAR;

            if (bib->leafInfo->simple == 1)
            {
               entry->typeExp.tag = bib->leafInfo->dataType.ttype;
               entry->width = entry->typeExp.tag == TK_INT ? 2 : 4;
               offafterin = entry->offset + entry->width;
            }
            else
            {
               entry->typeExp.ruid = bib->leafInfo->dataType.rtype;
               for (int i = 0; i < cwrap->length; i++)
               {
                  if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].ruid))
                  {
                     entry->width = cwrap->ctypes[i].typewidth;
                     break;
                  }
                  int l = 0;
                  for (int j = 0; j < cwrap->ctypes[i].numalias; j++)
                  {
                     if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].aliases[j]))
                     {
                        entry->width = cwrap->ctypes[i].typewidth;
                        offafterin = entry->offset + entry->width;
                        l = 1;
                        break;
                     }
                  }
                  if (l == 1)
                     break;
               }
            }
         }
      }

      //printf("got %s\n", ast->leafInfo->value.idPtr);
      // set scopename to funid
      // printf("FUNC NAME : %s\n", ast->leafInfo->value.idPtr);
      astNode *sib = ast->rightSibling->rightSibling;
      if (sib->nodeName == PARAMETER_LIST)
         sib = sib->rightSibling; // sib is now at stmts
                                  // printf("HERE : %s\n", getStringOf(sib->nodeName));
      sib = sib->leftChild;
      if (sib->nodeName == TYPEDEFINITIONS)
         sib = sib->rightSibling; // sib may now be at declarations (there may be no declarations)
      if (sib->nodeName == DECLARATIONS)
      {
         sib = sib->leftChild; // sib is now at first declaration
         for (; sib != NULL; sib = sib->rightSibling)
         { // iterating over declarations
            symbolTable *table;

            if (sib->leafInfo->isGlobal)
            {
               table = globalTable;
               table->scopeName = strdup("global");
            }
            else
            {
               table = &(localTables[index]);

               //printf("%d,%s\n", index, table->scopeName);
            }

            symbolTableEntry *entry;

            if (table->symbolTableEntries == NULL)
            {
               table->symbolTableEntries = (symbolTableEntry *)malloc(sizeof(symbolTableEntry));
               entry = table->symbolTableEntries;
               entry->offset = 0;
            }
            else
            {
               entry = table->symbolTableEntries;
               int total = 0;
               while (entry->nextEntry != NULL)
               {
                  total += entry->offset;
                  entry = entry->nextEntry;
               }
               entry->nextEntry = (symbolTableEntry *)malloc(sizeof(symbolTableEntry));
               entry->nextEntry->offset = table == globalTable ? total + entry->width : offafterin;
               entry = entry->nextEntry;
            }

            entry->name = sib->leafInfo->value.idPtr;
            if (table == globalTable)
               entry->scopNat = GLOBAL;
            else
            {
               entry->scopNat = LOCAL;
               entry->us = LOCALVAR;
            }
            if (sib->leafInfo->simple == 1)
            {
               entry->typeExp.tag = sib->leafInfo->dataType.ttype;
               entry->width = entry->typeExp.tag == TK_INT ? 2 : 4;
            }
            else
            {
               entry->typeExp.ruid = sib->leafInfo->dataType.rtype;
               for (int i = 0; i < cwrap->length; i++)
               {
                  if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].ruid))
                  {
                     entry->width = cwrap->ctypes[i].typewidth;
                     break;
                  }
                  int l = 0;
                  for (int j = 0; j < cwrap->ctypes[i].numalias; j++)
                  {
                     if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].aliases[j]))
                     {
                        entry->width = cwrap->ctypes[i].typewidth;
                        l = 1;
                        break;
                     }
                  }
                  if (l == 1)
                     break;
               }
            }
         }
      }
   }
}

void printSymbolTable(symbolTable *table, conTypeWrapper *cwrap)
{
   symbolTableEntry *entry = table->symbolTableEntries;
   printf("scopeName: %s\n", table->scopeName);
   while (entry != NULL)
   {
      printf("\t%s", entry->name);
      printf("\tscopeName: %s", table->scopeName);
      if (entry->typeExp.tag == TK_INT || entry->typeExp.tag == TK_REAL)
      {
         printf("\t---\t%s", getStringOf(entry->typeExp.tag));
      }
      else if (entry->typeExp.ruid)
      {
         printf("\t\t%s", entry->typeExp.ruid);
         for (int i = 0; i < cwrap->length; i++)
         {
            if ((!strcmp((*cwrap).ctypes[i].ruid, entry->typeExp.ruid)))
            {
               printf("\t<");
               contypexp *texp = (*cwrap).ctypes[i].typexp;
               while (texp != NULL)
               {
                  printf("%s", (texp->type.ttype >= 0 && texp->type.ttype < TERMTYPESIZE) ? getStringOf(texp->type.ttype) : texp->type.rtype);
                  texp = texp->next;
                  if (texp != NULL)
                     printf(", ");
                  else
                     printf(">");
               }
               break;
            }
            int l = 0;
            for (int j = 0; j < cwrap->ctypes[i].numalias; j++)
            {
               if (!strcmp(entry->typeExp.ruid, cwrap->ctypes[i].aliases[j]))
               {
                  printf("\t<");
                  contypexp *texp = (*cwrap).ctypes[i].typexp;
                  while (texp != NULL)
                  {
                     printf("%s", (texp->type.ttype >= 0 && texp->type.ttype < TERMTYPESIZE) ? getStringOf(texp->type.ttype) : texp->type.rtype);
                     texp = texp->next;
                     if (texp != NULL)
                        printf(", ");
                     else
                        printf(">");
                  }
                  l = 1;
                  break;
               }
            }
            if (l == 1)
               break;
         }
      }
      printf("\twid:%d", entry->width);
      printf("\t%s", (entry->scopNat == GLOBAL ? "GLOBAL" : "LOCAL"));
      printf("\toff:%d", entry->offset);
      printf("\tusage:%s\n", (entry->us == INPUTPAR ? "INPUTPAR" : (entry->us == OUTPUTPAR ? "OUTPUTPAR" : "LOCAL")));
      entry = entry->nextEntry;
   }
}

// symbolTable* makeTable(astNode* ast, symbolTable* parentTable) {
//    // assuming that we get the "root" / "program" node as input to this function

// }
