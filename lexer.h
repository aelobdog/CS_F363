// Group number 13
// Ashwin Kiran Godbole 2018B5A70423P
// Samarth Krishna Murthy 2018B2A70362P

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "lexerDef.h"

#define HASHTABLESIZE 512

// loadBuffer : loads the given buffer with TWIN_BUF_SINGLE_CAP amount memory read from given file
FILE* loadBuffer(char*, FILE*, int*);

// getToken : extract a token from the input stream
token getToken(twinBuffer*, hashTableEntry*);

// getTokenList : extracts and writes the list of tokens from the source file
void getTokenList(twinBuffer*, hashTableEntry*, token*);

// getAndPrintTokenList : extracts and prints the list of tokens from the source file
void getAndPrintTokenList(twinBuffer*, hashTableEntry*, token*);

// tokenTypeName : get the name of the token type
char* tokenTypeName(termType);

// tokenTypeValue : get the lexeme of the token type
char* tokenTypeValue(termType);

// prettyToken : pretty print token information to the console
void prettyToken(token);

// prettyHeading : display the heading for token list
void prettyHeading();

// initGlobalHashTable : to initialize the global hash table
void initGlobalHashTable(hashTableEntry*);

// initLexerDefaults : reads source code from specified file, loads its contents into the given twin buffer,
// initializes said buffer with default values, initializes the global hash table given and extracts the token list,
// into given token list 
void initLexerDefaults(char*, twinBuffer*, int*, hashTableEntry*, tokenList*);

// removeComments : function to clean the file (removing all the comments)
void removeComments(char*, char*);

// consolePrintNoComments : print the comment free code to the console
void consolePrintNoComments(char*);

// getTypeOf : to get the termType for the given string
termType getTypeOf(char*);

// getStringOf : to get the string value corresponding to the termType
char* getStringOf(termType type);

#endif
