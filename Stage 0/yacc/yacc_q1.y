%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);

int lvl = 0;
int max_lvl = 0;
%}

%token IF
%token ELSE
%token ID
%token SEMICOLON
%token LBRACE RBRACE

%%

start : statement_list { printf("\nComplete: max nesting level = %d\n", max_lvl); exit(0); }
    ;

statement_list: statement
              | statement_list statement
              ;

statement: simple_stmt
         | if_stmt
         ;

simple_stmt: ID SEMICOLON { printf(";\n"); }
           ;

if_stmt: IF LBRACE {
             lvl++;
             if(max_lvl < lvl) max_lvl = lvl;
             printf("IF level %d\n", lvl);
         } statement_list RBRACE ELSE LBRACE statement_list RBRACE {
             lvl--;
         }
       ;

%%

void yyerror(const char *s)
{
    printf("Error: %s\n", s);
}

int yylex(void)
{
    int c;

    while ((c = getchar()) == ' ' || c == '\t' || c == '\n') {}

    if (c == EOF) return 0;

    if (isalpha(c)) {
        char buffer[32];
        int i = 0;
        buffer[i++] = c;

        while ((c = getchar()) != EOF && isalnum(c)) {
            buffer[i++] = c;
        }
        buffer[i] = '\0';

        if (c != EOF)
            ungetc(c, stdin); 

        if (strcmp(buffer, "if") == 0)
            return IF;
            
        if (strcmp(buffer, "else") == 0)
        	return ELSE;

        return ID;
    }

    if (c == ';') return SEMICOLON;
    if (c == '{') return LBRACE;
    if (c == '}') return RBRACE;

    return c;
}


int main(void)
{
    printf("Enter your code (e.g., if{ a; if{ b; } } ):\n");
    yyparse();
    return 0;
}
