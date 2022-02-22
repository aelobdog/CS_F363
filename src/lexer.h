#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "lexerDef.h"

// loadBuffer : loads the given buffer with 4K memory read from given file
FILE* loadBuffer(char*, FILE*, int*);

// getToken : extract a token from the input stream
token getToken(twinBuffer*, hashTableEntry*);

// tokenTypeName : get the name of the token type
char* tokenTypeName(tokenType);

// prettyToken : pretty print token information to the console
void prettyToken(token);

// prettyHeading : display the heading for token list
void prettyHeading();

// initGlobalHashTable : to initialize the global hash table
void initGlobalHashTable(hashTableEntry*);

#endif
