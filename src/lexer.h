#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "lexerDef.h"

#define HASHTABLESIZE 512

// loadBuffer : loads the given buffer with 4K memory read from given file
FILE* loadBuffer(char*, FILE*, int*);

// getToken : extract a token from the input stream
token getToken(twinBuffer*, hashTableEntry*);

// getTokenList : extracts and writes the list of tokens from the source file
void getTokenList(twinBuffer* b, hashTableEntry* globalHashTable, token* tokenList);

// tokenTypeName : get the name of the token type
char* tokenTypeName(tokenType);

// prettyToken : pretty print token information to the console
void prettyToken(token);

// prettyHeading : display the heading for token list
void prettyHeading();

// initGlobalHashTable : to initialize the global hash table
void initGlobalHashTable(hashTableEntry*);

// removeComments : function to clean the file (removing all the comments)
void removeComments(char*, char*);

// consolePrintNoComments : print the comment free code to the console
void consolePrintNoComments(char*);


#endif
