%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "translate.c"
    int first = 1, addr = 2040, label = 0, flag = 0;

    FILE *fp;
%}

%%

"L"[0-9]+[:]  {
                    label = 1;
                    char *label_name = malloc(strlen(yytext));
                    strcpy(label_name, yytext);
                    label_name[strlen(yytext) - 1] = '\0';
                    if (first) {
                        append(label_name, addr);
                        addr -= 2;
                    }
                }

"JZ R"[0-9]+[,]" L"[0-9]+ {
                    if (!first) {
                        if (yytext[5] == ',') {
                            fprintf(fp, "JZ R%c, %d", yytext[4], getAddr(yytext + 7));
                        }
                        else {
                            fprintf(fp, "JZ R%c%c, %d", yytext[4], yytext[5], getAddr(yytext + 8));
                        }
                    }
                }

"JNZ R"[0-9]+[,]" L"[0-9]+ {
                    if (!first) {
                        if (yytext[6] == ',') {
                            fprintf(fp, "JNZ R%c, %d", yytext[5], getAddr(yytext + 8));
                        }
                        else {
                            fprintf(fp, "JNZ R%c%c, %d", yytext[5], yytext[6], getAddr(yytext + 9));
                        }
                    }
                }

"JMP L"[0-9]+ {
                    if (!first) {
                        fprintf(fp, "JMP %d", getAddr(yytext + 4));
                    }
                }

"\n" {
    addr += 2;
    if (!first) {
        fprintf(fp, "\n");
        label = 0;
    }
}

. {
    if (!first) {
        fprintf(fp, "%s", yytext);
    }
}

%%

int yywrap() {
    if (first) {
        yyin = fopen("output.xsm", "r");
        first = 0;
        return 0;
    }
    if (!first) {
        return 1;
    }
}

int main() {
    fp = fopen("code.xsm", "w");
    extern FILE *yyin;
    yyin = fopen("output.xsm", "r");
    yylex();
    return 0;
}
