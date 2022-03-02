// Group number 13
// Ashwin Kiran Godbole 2018B5A70423P
// Samarth Krishna Murthy 2018B2A70362P

#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdint.h>
typedef uint64_t u64;

#define MAX_TOKENS 2500

#define TWIN_BUF_SINGLE_CAP 4096

typedef enum termType {
    // TK_EOF,
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
    TK_NE,

    // non token terms
    PROGRAM,
    MAINFUNCTION,
    OTHERFUNCTION,
    FUNCTION,
    INPUT_PAR,
    OUTPUT_PAR,
    PARAMETER_LIST,
    DATATYPE,
    PRIMITIVEDATATYPE,
    CONSTRUCTEDDATATYPE,
    REMAINING_LIST,
    STMTS,
    TYPEDEFINITIONS,
    ACTUALORREDEFINED,
    TYPEDEFINITION,
    FIELDDEFINITIONS,
    FIELDDEFINITION,
    FIELDTYPE,
    MOREFIELDS,
    DECLARATIONS,
    DECLARATION,
    GLOBAL_OR_NOT,
    OTHERSTMTS,
    STMT,
    ASSIGNMENTSTMT,
    SINGLEORRECID,
    CONSTRUCTEDVARIABLE,
    ONEEXPANSION,
    MOREEXPANSIONS,
    FUNCALLSTMT,
    OUTPUTPARAMETERS,
    INPUTPARAMETERS,
    ITERATIVESTMT,
    CONDITIONALSTMT,
    ELSEPART,
    IOSTMT,
    ARITHMETICEXPRESSION,
    EXPPRIME,
    TERM,
    TERMPRIME,
    FACTOR,
    HIGHPRECEDENCEOPERATOR,
    LOWPRECEDENCEOPERATORS,
    BOOLEANEXPRESSION,
    VAR,
    LOGICALOP,
    RELATIONALOP,
    RETURNSTMT,
    OPTIONALRETURN,
    IDLIST,
    MORE_IDS,
    DEFINETYPESTMT,
    A,
    EPS,
    
    TERMTYPESIZE,
    DOLLAR // to mark the end of the token stream
} termType;

typedef struct termValName {
    termType tType; 
    char* tVal;
} termValName;

// token values
typedef union tokenValue {
    u64 num;
    double rnum;
    char* idPtr;
} tokenValue;

typedef struct token {
    u64 line;
    termType type;
    tokenValue value;
} token;

// struct containing a list of (MAX_TOKEN) tokens, and an index that holds the 'current'
// element being accessed (default = 0)
typedef struct tokenList {
    token list[MAX_TOKENS];
    int current;
} tokenList;

// twinBuffer
typedef struct twinBuffer {
    FILE* source;
    char buffer1[TWIN_BUF_SINGLE_CAP];

    char* lbBuf;
    char* fBuf;

    char* lexemeBegin;
    char* forward;

    char buffer2[TWIN_BUF_SINGLE_CAP];
    u64 currentLine;
} twinBuffer;

typedef struct hashTableEntry {
    int hash;
    token* Tptr;
} hashTableEntry;

#endif
