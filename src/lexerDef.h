#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdint.h>
typedef uint64_t u64;

// token types
typedef enum tokenType {
    TK_EOF,
    TK_ASSIGNOP,
    TK_COMMENT,
    TK_FIELDID,
    TK_ID,
    TK_NUM,
    TK_RNUM,
    TK_FUNID,
    TK_RUID,
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_SQL,
    TK_SQR,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_ENDWHILE,
    TK_OP,
    TK_CL,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE
} tokenType;

// token values
typedef union tokenValue {
    u64 num;
    double rnum;
    char* idPtr;
} tokenValue;

// twinBuffer
typedef struct twinBuffer {
    char buffer1[4096];
    char buffer2[4096];

    char* lbBuf;
    char* fBuf;

    char* lexemeBegin;
    char* forward;

    u64 currentLine;
} twinBuffer;

typedef struct token {
    u64 line;
    tokenType type;
    tokenValue value;
} token;

typedef struct hashTableEntry {
    int hash;
    token* Tptr;
} hashTableEntry;

#endif