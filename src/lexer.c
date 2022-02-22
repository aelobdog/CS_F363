#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"

#define MAX_LITERAL_LEN 32
#define HASHTABLESIZE 512

void initHashTable(hashTableEntry* ht, u64 size) {
    while (size > 0) {
        ht->Tptr = NULL;
        ht++;
        size--;
    }
}

token* allocAndReturnTokenPtr(token T) {
    token* TPtr = (token*)malloc(sizeof(token));
    TPtr->line = T.line;
    TPtr->type = T.type;
    TPtr->value.idPtr = strdup(T.value.idPtr);
    return TPtr;
}

u64 hash(char *str) {
    u64 hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASHTABLESIZE;
}

int lookup(hashTableEntry* ht, u64 size, u64 hash) {
    if (ht[hash].Tptr == NULL) return 0; // not present
    if (ht[hash].hash == hash) return 1;  // present
    hashTableEntry* iter = ht;
    for (; iter->Tptr != NULL && iter < ht+size; iter++) {
        if (iter->hash == hash) return 1;  // present
    }
    return 0; // not present
}

void insert(hashTableEntry* ht, u64 size, u64 hash, token T) {
    if (ht[hash].Tptr == NULL) {
        ht[hash].Tptr = allocAndReturnTokenPtr(T);
        // printf("In insert : %s\n", ht[hash].Tptr->value.idPtr);
        ht[hash].hash = hash;
    }

    if (ht[hash].hash == hash) {
        // check for duplication and shit
    }

    hashTableEntry* iter = ht;
    for (; iter->Tptr != NULL && iter < ht+size; iter++) {
        if (iter->hash == hash) {
            // check for duplication and shit
        }
    }
    
    iter->Tptr = allocAndReturnTokenPtr(T);
    iter->hash = hash;
}

void initGlobalHashTable(hashTableEntry* globalHashTable) {
    u64 tHash;
    token T;
    initHashTable(globalHashTable, HASHTABLESIZE);

    tHash = hash("endif");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_ENDIF; T.value.idPtr = strdup("endif");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("call");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_CALL; T.value.idPtr = strdup("call");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    T.value.idPtr = NULL;
    free(T.value.idPtr);

    tHash = hash("with");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_WITH; T.value.idPtr = strdup("with");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("parameters");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_PARAMETERS; T.value.idPtr = strdup("parameters");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("end");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_END; T.value.idPtr = strdup("end");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("while");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_WHILE; T.value.idPtr = strdup("while");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("union");
    //printf("hash of union : %ld\n", tHash);
    T.line = 0; T.type = TK_UNION; T.value.idPtr = strdup("union");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    T.value.idPtr = NULL;
    free(T.value.idPtr);

    tHash = hash("endunion");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_ENDUNION; T.value.idPtr = strdup("endunion");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("definetype");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_DEFINETYPE; T.value.idPtr = strdup("definetype");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("as");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_AS; T.value.idPtr = strdup("as");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("type");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_TYPE; T.value.idPtr = strdup("type");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("_main");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_MAIN; T.value.idPtr = strdup("_main");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("global");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_GLOBAL; T.value.idPtr = strdup("global");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("parameter");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_PARAMETER; T.value.idPtr = strdup("parameter");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("list");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_LIST; T.value.idPtr = strdup("list");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("input");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_INPUT; T.value.idPtr = strdup("input");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("output");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_OUTPUT; T.value.idPtr = strdup("output");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("int");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_INT; T.value.idPtr = strdup("int");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("real");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_REAL; T.value.idPtr = strdup("real");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("endwhile");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_ENDWHILE; T.value.idPtr = strdup("endwhile");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("if");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_IF; T.value.idPtr = strdup("if");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("then");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_THEN; T.value.idPtr = strdup("then");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("read");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_READ; T.value.idPtr = strdup("read");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("write");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_WRITE; T.value.idPtr = strdup("write");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("return");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_RETURN; T.value.idPtr = strdup("return");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("record");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_RECORD; T.value.idPtr = strdup("record");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("endrecord");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_ENDRECORD; T.value.idPtr = strdup("endrecord");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;

    tHash = hash("else");
    //printf("hash : %ld\n", tHash);
    T.line = 0; T.type = TK_ELSE; T.value.idPtr = strdup("else");
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
    T.value.idPtr = NULL;
}


int isKeyword(token t) {
    switch(t.type) {
        case TK_WITH:
        case TK_PARAMETERS:
        case TK_END:
        case TK_WHILE:
        case TK_UNION:
        case TK_ENDUNION:
        case TK_DEFINETYPE:
        case TK_AS:
        case TK_TYPE:
        case TK_MAIN:
        case TK_GLOBAL:
        case TK_PARAMETER:
        case TK_LIST:
        case TK_INPUT:
        case TK_OUTPUT:
        case TK_INT:
        case TK_REAL:
        case TK_ENDWHILE:
        case TK_THEN:
        case TK_ENDIF:
        case TK_READ:
        case TK_WRITE:
        case TK_RETURN:
        case TK_CALL:
        case TK_RECORD:
        case TK_ENDRECORD:
        case TK_ELSE: return 1;
        default: return 0;
    }
}

// -------------------------------------

int isDigit(char c) {
    if (c>='0' && c<='9') return 1;
    return 0;
}

void reportLexError(char* tokenName) {
    printf("LEX ERROR : Unknown Token `%s`\n", tokenName);
    exit(1);
}

FILE* loadBuffer(char* buffer, FILE* source, int *eof) {
    fread(buffer, 4096, 1, source);
    *eof = feof(source);
    if (*eof) *eof = 1;
    return source;
}

void incForward(twinBuffer *b) {
    if (b->forward - b->fBuf == 4095) {
        if (b->fBuf == b->buffer1) b->fBuf = b->buffer2;
        else b->fBuf = b->buffer1;
        b->forward = b->fBuf;
    }
    else {
        b->forward++;
    }
}

void decForward(twinBuffer *b) {
    if (b->forward == b->fBuf) {
        if (b->fBuf == b->buffer1) b->fBuf = b->buffer2;
        else b->fBuf = b->buffer1;
        b->forward = b->fBuf + 4095;
    }
    else {
        b->forward--;
    }
}

char* tokenTypeName(tokenType type) {
    char* string;
    switch(type) {
    case TK_EOF: string = "TK_EOF"; break;
    case TK_ASSIGNOP: string = "TK_ASSIGNOP"; break;
    case TK_COMMENT: string = "TK_COMMENT"; break;
    case TK_FIELDID: string = "TK_FIELDID"; break;
    case TK_ID: string = "TK_ID"; break;
    case TK_NUM: string = "TK_NUM"; break;
    case TK_RNUM: string = "TK_RNUM"; break;
    case TK_FUNID: string = "TK_FUNID"; break;
    case TK_RUID: string = "TK_RUID"; break;
    case TK_WITH: string = "TK_WITH"; break;
    case TK_PARAMETERS: string = "TK_PARAMETERS"; break;
    case TK_END: string = "TK_END"; break;
    case TK_WHILE: string = "TK_WHILE"; break;
    case TK_UNION: string = "TK_UNION"; break;
    case TK_ENDUNION: string = "TK_ENDUNION"; break;
    case TK_DEFINETYPE: string = "TK_DEFINETYPE"; break;
    case TK_AS: string = "TK_AS"; break;
    case TK_TYPE: string = "TK_TYPE"; break;
    case TK_MAIN: string = "TK_MAIN"; break;
    case TK_GLOBAL: string = "TK_GLOBAL"; break;
    case TK_PARAMETER: string = "TK_PARAMETER"; break;
    case TK_LIST: string = "TK_LIST"; break;
    case TK_SQL: string = "TK_SQL"; break;
    case TK_SQR: string = "TK_SQR"; break;
    case TK_INPUT: string = "TK_INPUT"; break;
    case TK_OUTPUT: string = "TK_OUTPUT"; break;
    case TK_INT: string = "TK_INT"; break;
    case TK_REAL: string = "TK_REAL"; break;
    case TK_COMMA: string = "TK_COMMA"; break;
    case TK_SEM: string = "TK_SEM"; break;
    case TK_COLON: string = "TK_COLON"; break;
    case TK_DOT: string = "TK_DOT"; break;
    case TK_ENDWHILE: string = "TK_ENDWHILE"; break;
    case TK_OP: string = "TK_OP"; break;
    case TK_CL: string = "TK_CL"; break;
    case TK_IF: string = "TK_IF"; break;
    case TK_THEN: string = "TK_THEN"; break;
    case TK_ENDIF: string = "TK_ENDIF"; break;
    case TK_READ: string = "TK_READ"; break;
    case TK_WRITE: string = "TK_WRITE"; break;
    case TK_RETURN: string = "TK_RETURN"; break;
    case TK_PLUS: string = "TK_PLUS"; break;
    case TK_MINUS: string = "TK_MINUS"; break;
    case TK_MUL: string = "TK_MUL"; break;
    case TK_DIV: string = "TK_DIV"; break;
    case TK_CALL: string = "TK_CALL"; break;
    case TK_RECORD: string = "TK_RECORD"; break;
    case TK_ENDRECORD: string = "TK_ENDRECORD"; break;
    case TK_ELSE: string = "TK_ELSE"; break;
    case TK_AND: string = "TK_AND"; break;
    case TK_OR: string = "TK_OR"; break;
    case TK_NOT: string = "TK_NOT"; break;
    case TK_LT: string = "TK_LT"; break;
    case TK_LE: string = "TK_LE"; break;
    case TK_EQ: string = "TK_EQ"; break;
    case TK_GT: string = "TK_GT"; break;
    case TK_GE: string = "TK_GE"; break;
    case TK_NE: string = "TK_NE"; break;
    }

    return string;
}

void prettyHeading() {
   printf("\n%20s\t%13s\t%5s\n", "type", "value", "line");
   printf("%20s\t%13s\t%5s\n", "----", "-----", "----");
}

void prettyToken(token T) {
    printf("%20s\t", tokenTypeName(T.type));
    switch (T.type){
        case TK_NUM:  printf("%13ld\t", T.value.num); break;
        case TK_RNUM: printf("%13.2lf\t", T.value.rnum); break;
        
        default: printf("%13s\t", T.value.idPtr); break;
    }
    printf("%5ld\n", T.line);
}

void setTokenValue(token* T, char* charBuf, int cbPtr) {
    switch (T->type)
    {
    // setting the token value to 0 because for these tokens, 
    // the token's type provides all the necessary information about the token
    case TK_EQ:
    case TK_NE:
    case TK_NOT:
    case TK_AND:
    case TK_OR:
    case TK_DIV:
    case TK_MUL:
    case TK_PLUS:
    case TK_MINUS:
    case TK_OP: 
    case TK_CL:
    case TK_DOT:
    case TK_COLON:
    case TK_SEM:
    case TK_COMMA:
    case TK_SQL:
    case TK_SQR:
    case TK_GT:
    case TK_GE:
    case TK_LT:
    case TK_LE:
    case TK_ASSIGNOP:
        T->value.num = 0;
        break;
    
    case TK_NUM:
        sscanf(charBuf, "%ld", &(T->value.num));
        break;

    case TK_RNUM:
        sscanf(charBuf, "%lf", &(T->value.rnum));
        break;

    default:
        T->value.idPtr = strdup(charBuf);
    }   
}

token getToken(twinBuffer* b, hashTableEntry* globalHashTable) {
    token T;
    char charBuf[MAX_LITERAL_LEN] = {0};
    int cbPtr;
    int state;
    int inProgress;
    char c;

    cbPtr = 0;
    state = 0;
    inProgress = 1;

    while(inProgress) {
        // if (b->forward != b.fBuf) b->forward --;
        c = *(b->forward);
        // printf("c = %c, state = %d\n",c,state);
        // printf("on line : %ld\n", b->currentLine);
        incForward(b);

        // if (c == '\n') {
        //     b->currentLine ++;
        // }
        
        switch(state) {
        case 0:
            if (!(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '%')) charBuf[cbPtr++] = c;
            switch(c) {
                case '_': state = 1;  break;
                case '<': state = 7;  break;
                case '>': state = 13; break;
                case '#': state = 26; break;
                case '=': state = 35; break;
                case '!': state = 37; break;
                case '~': state = 39; break;
                case '&': state = 40; break;
                case '@': state = 43; break;
                case '/': state = 46; break;
                case '*': state = 47; break;
                case '+': state = 48; break;
                case '-': state = 49; break;
                case '(': state = 50; break;
                case ')': state = 51; break;
                case '.': state = 52; break;
                case ':': state = 53; break;
                case ';': state = 54; break;
                case ',': state = 55; break;
                case '[': state = 56; break;
                case ']': state = 57; break;
                case '%': state = 58; break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9': state = 16; break;
                case 'b': 
                case 'c': 
                case 'd': state = 29; break;
                case 'a':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z': state = 33; break;
                
                case '\n':
                    if (c == '\n') {
                        b->currentLine ++;
                    }
                case  ' ': 
                case '\r':
                case '\t': state = 60; break;

                default: exit(0);
            }
            break;

        case 1:
            charBuf[cbPtr++] = c;
            if(c >= 'A' && c <= 'Z') state = 4;
            else if(c >= 'a' && c <= 'z') state = 2;
            else reportLexError(charBuf);
            break;
        case 2:
            charBuf[cbPtr++] = c;
            if(c >= 'A' && c <= 'Z') state = 4;
            else if(isDigit(c)) state = 5;
            else if(!(c >= 'a' && c <= 'z')) state = 3;
            break;
        case 3:
            //search
        case 4:
            charBuf[cbPtr++] = c;
            if(c >= '0' && c <= '9') state = 5;
            else if(!(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) reportLexError(charBuf); 
            break;
        case 5:
            charBuf[cbPtr++] = c;
            if(!(c >= '0' && c <= '9')) state = 6;
            break;
        case 6:
            //search
        case 7:
            charBuf[cbPtr++] = c;
            if(c == '-') state = 8;
            else if (c == '=') state = 11;
            else state = 12;
            break;
        case 8:
            charBuf[cbPtr++] = c;
            if(c == '-') state = 9;
            else reportLexError(charBuf);
            break;
        case 9:
            charBuf[cbPtr++] = c;
            if(c == '-') state = 10;
            else reportLexError(charBuf);
            break;
        case 10:
            T.type = TK_ASSIGNOP;
            inProgress = 0;
            decForward(b);
            break;
        case 11:
            T.type = TK_LE;
            inProgress = 0;
            decForward(b);
            break;
        case 12:
            T.type = TK_LT;
            inProgress = 0;
            decForward(b);
            decForward(b);
            break;
        case 13:
            charBuf[cbPtr++] = c;
            if (c == '=') state = 14;
            else state = 15;
            break;
        case 14:
            T.type = TK_GE;
            inProgress = 0;
            decForward(b);
            break;
        case 15:
            T.type = TK_GT;
            inProgress = 0;
            decForward(b);
            decForward(b);
            break;
        case 16: 
            charBuf[cbPtr++] = c;
            if (c == '.') {
                state = 17;
                break;
            }
            if (!isDigit(c)) {
                state = 25;
                break;
            }
            break;
        case 17:
            charBuf[cbPtr++] = c;
            if (isDigit(c)) state = 18;
            else reportLexError(charBuf);
            break;
        case 18:
            charBuf[cbPtr++] = c;
            if (isDigit(c)) state = 19;
            else reportLexError(charBuf);
            break;
        case 19:
            charBuf[cbPtr++] = c;
            if (c == 'E') state = 21;
            else state = 20;
            break;
        case 20:
            T.type = TK_RNUM;
            inProgress = 0;
            decForward(b);
            decForward(b);
            break;
        case 21:
            charBuf[cbPtr++] = c;
            if (c == '+' || c == '-') state = 22;
            else if (isDigit(c)) state = 23;
            else reportLexError(charBuf);
            break;
        case 22:
            charBuf[cbPtr++] = c;
            if (isDigit(c)) state = 23;
            else reportLexError(charBuf);
            break;
        case 23:
            charBuf[cbPtr++] = c;
            if (isDigit(c)) state = 24;
            else reportLexError(charBuf);
            break;
        case 24:
            T.type = TK_RNUM;
            inProgress = 0;
            decForward(b);
            break;
        case 25:
            T.type = TK_NUM;
            inProgress = 0;
            decForward(b);
            decForward(b);
            break;
        case 26:
            charBuf[cbPtr++] = c;
            if(c >= 'a' && c <= 'z') state = 27;
            else reportLexError(charBuf);
            break;
        case 27:
            charBuf[cbPtr++] = c;
            if(!(c >= 'a' && c <= 'z')) state = 28;
            break;
        case 28:
            //search
        case 29:
            charBuf[cbPtr++] = c;
            if(c >= 'a' && c <= 'z') state = 33;
            else if(c >= '2' && c <= '7') state = 30;
            else state = 34;
            break;
        case 30:
            charBuf[cbPtr++] = c;
            if(c >= '2' && c <= '7') state = 31;
            else if(!(c >= 'b' && c <= 'd')) state = 32;
            break;
        case 31:
            charBuf[cbPtr++] = c;
            if(!(c >= '2' && c <= '7')) state = 32;
            break;
        case 32:
            // search
        case 33:
            if (!(c >= 'a' && c <= 'z')) {
                state = 34;
                break;
            }
            charBuf[cbPtr++] = c;
            break;
        case 34:
            // printf("got some string %s\n", charBuf);
            inProgress = 0;
            decForward(b);
            decForward(b);
            u64 tHash = hash(charBuf);
            // printf("charBuf = %s\n", charBuf);
            // printf("looking for hash %ld\n", hash(charBuf));
            int found = lookup(globalHashTable, HASHTABLESIZE, hash(charBuf));
            //printf("%d\n", found);
            if (found && !strcmp(globalHashTable[tHash].Tptr->value.idPtr, charBuf)) {
                // printf("%s and %s\n", globalHashTable[tHash].Tptr->value.idPtr, charBuf);
                T.type = globalHashTable[tHash].Tptr->type;
            } else {
                T.type = TK_ID; T.line = b->currentLine; T.value.idPtr = strdup(charBuf);
                insert(globalHashTable, HASHTABLESIZE, tHash, T);
            }
            break;
        case 35: 
            charBuf[cbPtr++] = c;
            if(c == '=') state = 36;
            else reportLexError(charBuf);
            break;
        case 36:
            T.type = TK_EQ;
            inProgress = 0;
            decForward(b);
            break;
        case 37:
            charBuf[cbPtr++] = c;
            if(c == '=') state = 38;
            else reportLexError(charBuf);
            break;
        case 38:
            T.type = TK_NE;
            inProgress = 0;
            decForward(b);
            break;
        case 39:
            T.type = TK_NOT;
            inProgress = 0;
            decForward(b);
            break;
        case 40:
            charBuf[cbPtr++] = c;
            if(c == '&') state = 41;
            else reportLexError(charBuf);
            break;
        case 41:
            charBuf[cbPtr++] = c;
            if(c == '&') state = 42;
            else reportLexError(charBuf);
            break;
        case 42:
            T.type = TK_AND;
            inProgress = 0;
            decForward(b);
            break;
        case 43:
            charBuf[cbPtr++] = c;
            if (c == '@') state = 44;
            else reportLexError(charBuf);
            break;
        case 44:
            charBuf[cbPtr++] = c;
            if (c == '@') state = 45;
            else reportLexError(charBuf);
            break;
        case 45:
            T.type = TK_OR;
            inProgress = 0;
            decForward(b);
            break;
        case 46:
            T.type = TK_DIV;
            inProgress = 0;
            decForward(b);
            break;
        case 47:
            T.type = TK_MUL;
            inProgress = 0;
            decForward(b);
            break;
        case 48:
            T.type = TK_PLUS;
            inProgress = 0;
            decForward(b);
            break;
        case 49:
            T.type = TK_MINUS;
            inProgress = 0;
            decForward(b);
            break;
        case 50:
            T.type = TK_OP;
            inProgress = 0;
            decForward(b);
            break;
        case 51:
            T.type = TK_CL;
            inProgress = 0;
            decForward(b);
            break;
        case 52:
            T.type = TK_DOT;
            inProgress = 0;
            decForward(b);
            break;
        case 53:
            T.type = TK_COLON;
            inProgress = 0;
            decForward(b);
            break;
        case 54:
            T.type = TK_SEM;
            inProgress = 0;
            decForward(b);
            break;
        case 55:
            T.type = TK_COMMA;
            inProgress = 0;
            decForward(b);
            break;
        case 56:
            T.type = TK_SQL;
            inProgress = 0;
            decForward(b);
            break;
        case 57:
            T.type = TK_SQR;
            inProgress = 0;
            decForward(b);
            break;
        case 58:
            if (c == '\n') {
                state = 59;
                b->currentLine ++;
            }
            break;
        case 59:
            decForward(b);
            state = 0;
            memset(charBuf,0,sizeof(charBuf));
            cbPtr = 0;
            break;

        case 60:
            if (!(c == ' ' || c == '\n' || c == '\t' || c == '\r'|| c == '\f')) state = 61;
            if (c == '\n') {
                b->currentLine ++;
            }
            break;
        
        case 61:
            decForward(b);
            decForward(b);
            state = 0;
            memset(charBuf,0,sizeof(charBuf));
            cbPtr = 0;
            break;

        default:
            reportLexError("unreachable state reached !");
            T.type = TK_EOF;
            return T;
        }
    }

    T.line = b->currentLine;
    // if (c == '\n') {
    //     b->currentLine ++;
    // }
    setTokenValue(&T, charBuf, cbPtr);
    return T;
}
