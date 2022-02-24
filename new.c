#include "src/lexer.h"

int main() {
    hashTableEntry globalHashTable[HASHTABLESIZE];
    twinBuffer b;
    FILE* source;
    token tokenList[2500];
    int eof;

   
    source = fopen("test.txt", "r");
    if (!source) perror("FILE READ ERROR : ");

    source = loadBuffer(b.buffer1, source, &eof);
    if (!eof) source = loadBuffer(b.buffer2, source, &eof);

    b.fBuf = b.buffer1;
    b.lbBuf = b.buffer1;
    b.forward = b.fBuf;
    b.lexemeBegin = b.lbBuf;
    b.currentLine = 1;

    initGlobalHashTable(globalHashTable);
    prettyHeading();
    getTokenList(&b, globalHashTable, tokenList);

    // for (int i=0; tokenList[i].type != TK_EOF; i++) prettyToken(tokenList[i]);
}
