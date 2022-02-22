#include "lexer.h"
#include <stdio.h>
// #include "lexerDef.h"
#define HASHTABLESIZE 512

int main() {
    // hash table stuff for the symbol table
    hashTableEntry globalHashTable[HASHTABLESIZE];

    twinBuffer b;
    FILE* source;
    int eof;

    source = fopen("test.txt", "r");
    if (!source) perror("file read");

    source = loadBuffer(b.buffer1, source, &eof);
    if (!eof) source = loadBuffer(b.buffer2, source, &eof);

    b.fBuf = b.buffer1;
    b.lbBuf = b.buffer1;
    b.forward = b.fBuf;
    b.lexemeBegin = b.lbBuf;
    b.currentLine = 1;

    token T;
    initGlobalHashTable(globalHashTable);
    prettyHeading();
    do {
        T = getToken(&b, globalHashTable);
        prettyToken(T);
    } while (T.type != TK_EOF);
    return 0;
}
