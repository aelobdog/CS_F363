#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexerDef.h"

#define MAX_LITERAL_LEN 32
#define HASHTABLESIZE 512

void removeComments(char *testcaseFile, char *cleanFile) {
    FILE* f = fopen(cleanFile,"w");
    FILE* g = fopen(testcaseFile,"r");
    char c;
    while(!feof(g)) {
        c = fgetc(g);
        if (c == '%') {
            c = fgetc(g);
            while(c != '\n') {
                if(feof(g)) return;
                c = fgetc(g);
            }
            continue;
        }
        fputc(c,f);
    }
    fclose(g);
    fclose(f);
}

void consolePrintNoComments(char* testcaseFile) {
    FILE* g = fopen(testcaseFile,"r");
    printf("filename : %s\n", testcaseFile);
    if (!g) { perror("FILE OPEN ERROR : "); }

    char c;
    while(! feof(g)) {
        c = fgetc(g);
        if(c == '%') {
            c = fgetc(g);
            while(c != '\n') {
                if(feof(g)) return;
                c = fgetc(g);
            }
            continue;
        }
        printf("%c",c);
    }
    fclose(g);
}

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

void InsertInGlobalHashTable(char* name, tokenType type, hashTableEntry* globalHashTable) {
    u64 tHash = hash(name);
    token T;
    T.line = 0; T.type = type; T.value.idPtr = strdup(name);
    insert(globalHashTable, HASHTABLESIZE, tHash, T);
    free(T.value.idPtr);
}

void initGlobalHashTable(hashTableEntry* globalHashTable) {
    u64 tHash;
    token T;
    initHashTable(globalHashTable, HASHTABLESIZE);

    InsertInGlobalHashTable("endif", TK_ENDIF, globalHashTable);
    InsertInGlobalHashTable("call", TK_CALL, globalHashTable);
    InsertInGlobalHashTable("with", TK_WITH, globalHashTable);
    InsertInGlobalHashTable("parameters", TK_PARAMETERS, globalHashTable);
    InsertInGlobalHashTable("end", TK_END, globalHashTable);
    InsertInGlobalHashTable("while", TK_WHILE, globalHashTable);
    InsertInGlobalHashTable("union", TK_UNION, globalHashTable);
    InsertInGlobalHashTable("endunion", TK_ENDUNION, globalHashTable);
    InsertInGlobalHashTable("definetype", TK_DEFINETYPE, globalHashTable);
    InsertInGlobalHashTable("as", TK_AS, globalHashTable);
    InsertInGlobalHashTable("type", TK_TYPE, globalHashTable);
    InsertInGlobalHashTable("global", TK_GLOBAL, globalHashTable);
    InsertInGlobalHashTable("parameter", TK_PARAMETER, globalHashTable);
    InsertInGlobalHashTable("list", TK_LIST, globalHashTable);
    InsertInGlobalHashTable("input", TK_INPUT, globalHashTable);
    InsertInGlobalHashTable("output", TK_OUTPUT, globalHashTable);
    InsertInGlobalHashTable("int", TK_INT, globalHashTable);
    InsertInGlobalHashTable("real", TK_REAL, globalHashTable);
    InsertInGlobalHashTable("endwhile", TK_ENDWHILE, globalHashTable);
    InsertInGlobalHashTable("if", TK_IF, globalHashTable);
    InsertInGlobalHashTable("then", TK_THEN, globalHashTable);
    InsertInGlobalHashTable("read", TK_READ, globalHashTable);
    InsertInGlobalHashTable("write", TK_WRITE, globalHashTable);
    InsertInGlobalHashTable("return", TK_RETURN, globalHashTable);
    InsertInGlobalHashTable("record", TK_RECORD, globalHashTable);
    InsertInGlobalHashTable("endrecord", TK_ENDRECORD, globalHashTable);
    InsertInGlobalHashTable("else", TK_ELSE, globalHashTable);
}


int isKeyword(token t) {
    switch(t.type) {
        case TK_WITH:           case TK_PARAMETERS:
        case TK_END:            case TK_WHILE:
        case TK_UNION:          case TK_ENDUNION:
        case TK_DEFINETYPE:     case TK_AS:
        case TK_TYPE:           case TK_MAIN:
        case TK_GLOBAL:         case TK_PARAMETER:
        case TK_LIST:           case TK_INPUT:
        case TK_OUTPUT:         case TK_INT:
        case TK_REAL:           case TK_ENDWHILE:
        case TK_THEN:           case TK_ENDIF:
        case TK_READ:           case TK_WRITE:
        case TK_RETURN:         case TK_CALL:
        case TK_RECORD:         case TK_ENDRECORD:
        case TK_ELSE: return 1;
        default: return 0;
    }
}

// -------------------------------------

#define isDigit(c) ((c>='0' && c<='9'))

void reportLexError(char* tokenName) {
    printf("LEX ERROR : Unknown Token `%s`\n", tokenName);
    exit(1);
}

FILE* loadBuffer(char* buffer, FILE* source, int *eof) {
    fread(buffer, 4096, 1, source);
    buffer[strlen(buffer)] = '\0';
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
        case TK_ID: case TK_RUID: case TK_FUNID: printf("%13s\t", T.value.idPtr); break;
        default: printf("%13s\t", tokenTypeName(T.type)); break;
    }
    printf("%5ld\n", T.line);
}

void setTokenValue(token* T, char* charBuf, int cbPtr) {
    switch (T->type)
    {
    // setting the token value to 0 because for these tokens, 
    // the token's type provides all the necessary information about the token
    case TK_EQ:     case TK_NE:
    case TK_NOT:    case TK_AND:
    case TK_OR:     case TK_DIV:
    case TK_MUL:    case TK_PLUS:
    case TK_MINUS:  case TK_OP: 
    case TK_CL:     case TK_DOT:
    case TK_COLON:  case TK_SEM:
    case TK_COMMA:  case TK_SQL:
    case TK_SQR:    case TK_GT:
    case TK_GE:     case TK_LT:
    case TK_LE:     case TK_ASSIGNOP:
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
        c = *(b->forward);
        incForward(b);

        // printf("c = %c, state = %d\n",c,state);
        // printf("on line : %ld\n", b->currentLine);
        
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
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
                    state = 16; break;

                case 'b': case 'c': case 'd':  
                    state = 29; break;

                case 'a': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': 
                case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': 
                    state = 33; break;
                
                case '\n':
                    if (c == '\n') {
                        b->currentLine ++;
                    }
                case  ' ': 
                case '\r':
                case '\t': state = 60; break;

                default: printf("here\n"); state = 100; break;
            }
            break;

        case 1:
            charBuf[cbPtr++] = c;
            if(c >= 'A' && c <= 'Z') state = 4;
            else if(c >= 'a' && c <= 'z') state = 2;
            else reportLexError(charBuf);
            break;
        case 2:
            if(c >= 'A' && c <= 'Z') state = 4;
            else if(isDigit(c)) state = 5;
            else if(!(c >= 'a' && c <= 'z')) { state = 6; break; }
            charBuf[cbPtr++] = c;
            break;
        case 3:
            // STATE NOT REQUIRED.
            break;
        case 4:
            if(isDigit(c)) state = 5;
            else if(!(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { state = 6; break; } 
            charBuf[cbPtr++] = c;
            break;  
        case 5:
            if(!isDigit(c)) { state = 6; break; }
            charBuf[cbPtr++] = c;
            break;
        case 6:
            inProgress = 0;
            decForward(b);
            decForward(b);
            if (!strcmp("_main", charBuf)) {
                T.type = TK_MAIN; T.value.idPtr = strdup("_main");
            } else {
                T.type = TK_FUNID; T.value.idPtr = strdup(charBuf);
            }
            break;
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
            if(!(c >= 'a' && c <= 'z')) { state = 28; break; }
            charBuf[cbPtr++] = c;
            break;
        case 28:
            T.type = TK_RUID;
            inProgress = 0;
            decForward(b);
            decForward(b);
            break;
        case 29:
            charBuf[cbPtr++] = c;
            if(c >= 'a' && c <= 'z') state = 33;
            else if(c >= '2' && c <= '7') state = 30;
            else state = 34;
            break;
        case 30:
            if(c >= '2' && c <= '7') state = 31;
            else if(!(c >= 'b' && c <= 'd')) { state = 32; break; }
            charBuf[cbPtr++] = c;
            break;
        case 31:
            if(!(c >= '2' && c <= '7')) { state = 32; break; }
            charBuf[cbPtr++] = c;
            break;
        case 32:
            inProgress = 0;
            decForward(b);
            decForward(b);
            T.type = TK_ID;
            break;
        case 33:
            if (!(c >= 'a' && c <= 'z')) {
                state = 34;
                break;
            }
            charBuf[cbPtr++] = c;
            break;
        case 34:
            inProgress = 0;
            decForward(b);
            decForward(b);
            u64 tHash = hash(charBuf);
            int found = lookup(globalHashTable, HASHTABLESIZE, hash(charBuf));
            if (found && !strcmp(globalHashTable[tHash].Tptr->value.idPtr, charBuf)) {
                T.type = globalHashTable[tHash].Tptr->type;
            } else {
                T.type = TK_FIELDID; T.line = b->currentLine; T.value.idPtr = strdup(charBuf);
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
            // reportLexError("unreachable state reached !");
            T.type = TK_EOF;
            return T;
        }
    }

    T.line = b->currentLine;
    setTokenValue(&T, charBuf, cbPtr);
    return T;
}

void getTokenList(twinBuffer* b, hashTableEntry* globalHashTable, token tokenList[]) {
   token T;
   int i = 0;
   do {
      T = getToken(b, globalHashTable);
      tokenList[i] = T;
      i++;
   } while (T.type != TK_EOF && i < 2500);
}
