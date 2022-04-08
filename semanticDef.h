#ifndef SEMANTICDEF_H
#define SEMANTICDEF_H

#include "parserDef.h"

typedef unsigned long ul;

typedef enum {
   INTEGER,
   REAL,
   RECORD,
   UNION
} dataType;

// int typewidths[2] = {4, 8};

typedef struct symbolTableEntry {
   char* name;
   dataType type;
   ul offset;

   struct symbolTableEntry* nextEntry; // to deal with collisions (post hashing)
} symbolTableEntry;

typedef struct symbolTable {
   char* scopeName; // could be a function name, or identifier for scopes of if, while, etc...
   symbolTableEntry* symbolTableEntries;
   ul tableOffset;
} symbolTable;

typedef struct astNode{
   symbolTableEntry* stEntry;
   termType nodeName;
   struct astNode* parent;
   struct astNode* rightSibling;
   struct astNode* leftChild;
   int depth;
} astNode;

#endif