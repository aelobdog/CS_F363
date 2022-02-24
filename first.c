#include <stdio.h>

typedef struct {
    token nonTerminal;
    token rhs[15];
} prodRule;

int main() {
    int rulenum = 0;
    int termnum = 0;
    char buf[100];
    char c;
    FILE* f = fopen("grammar.txt","r");
    prodRule rules[200] = {0};
    while (fscanf(f, "%s%c", buf, &c) == 2) {
        if (termnum == 0) {
            g[rownum].rule = string_to_token(buf);
        } else {
            g[rownum].rhs[termnum - 1] = string_to_token(buf);
        }
        rulenum++;
        if (c == '\n') {
            rulenum++;
            termnum = 0;
        }
    }
    fclose(f);
}