#ifndef SEMANTICDEF_H
#define SEMANTICDEF_H

#include "parserDef.h"

typedef enum {
   INTEGER,
   REAL,
   RECORD,
   UNION
} dataTypeTag;

typedef enum {
   LOCAL,
   GLOBAL
} scopeNature;

typedef enum {
   LOCALVAR,
   INPUTPAR,
   OUTPUTPAR
} usage;

typedef struct {
   dataTypeTag tag;
   struct dataTypeExp* type;
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
   struct symbolTable* parentTable;
   char* scopeName; // could be a function name, or identifier for scopes of if, while, etc...
   symbolTableEntry* symbolTableEntries;
   int tableOffset;
} symbolTable;

typedef struct astNode {
   termType nodeName;
   tokenValue value;
   termType dataType;
   char* ruid;
   signed char isGlobal; // 1 = true, 0 = false
   
   struct astNode* parent;
   struct astNode* rightSibling;
   struct astNode* leftChild;
   short depth;
} astNode;

#endif