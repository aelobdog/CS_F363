#ifndef SEMANTICDEF_H
#define SEMANTICDEF_H

#include "lexerDef.h"
#include "parserDef.h"

typedef enum {
   LOCAL,
   GLOBAL
} scopeNature;

typedef enum {
   LOCALVAR,
   INPUTPAR,
   OUTPUTPAR
} usage;

typedef struct dataTypeExp {
   termType tag;
   char* ruid;
} dataTypeExp;

typedef struct typeName {
   char* name;
   struct typeName* aliases;
} typeName;

typedef struct symbolTableEntry {
   char* name; // identifier value
   typeName* typeNames; // only for records (unions?)
   dataTypeExp typeExp; // full datatype
   int width; // space required by the variable
   scopeNature scopNat; // local ? global ?
   int offset; // prev offset + cur width
   
   struct symbolTableEntry* nextEntry; // to deal with collisions (post hashing)
} symbolTableEntry;

typedef struct symbolTable {
   // struct symbolTable* parentTable;
   char* scopeName; 
   symbolTableEntry* symbolTableEntries;
   int tableOffset;
} symbolTable;

typedef union Type {
      termType ttype;
      char* rtype;
} Type;

typedef struct contypexp {
   Type type;
   struct contypexp* next;
} contypexp;

typedef struct constructedTypes {
   termType ctype;
   char* ruid;
   int numalias;
   char* aliases[5];
   contypexp* typexp;
   long typewidth;
} constructedTypes;

typedef struct conTypeWrapper {
   constructedTypes ctypes[50];
   int length;
} conTypeWrapper;

typedef struct astNode {   
   struct astNode* parent;
   struct astNode* rightSibling;
   struct astNode* leftChild;
   struct astLeafInfo* leafInfo;
   termType nodeName;
   short depth;
} astNode;

typedef struct astLeafInfo {
   tokenValue value;
   // termType dataType;
   // char* ruid;
   int simple; // 1 = true, 0 = false
   Type dataType;
   int isGlobal; // 1 = true, 0 = false
} astLeafInfo;

typedef struct returnTup {
   int nodes;
   long size;
} returnTup;

#endif
