%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);
%}

%union {
    char *str;
}

/* Tokens */
%token <str> ID
%token NEWLINE

%left '+' '-'
%left '*' '/'

%type <str> expr

%%

input
    : 
    | input line
    ;

line
    : expr NEWLINE
        {
            printf("%s\n", $1);
            free($1);
        }
    | NEWLINE 
    ;

expr
    : ID
        {
            $$ = $1;
        }
    | expr '+' expr
        {
            size_t len = 2 + 1 + strlen($1) + 1 + strlen($3) + 1;
            $$ = (char*)malloc(len);
            if(!$$) { perror("malloc"); exit(1); }
            snprintf($$, len, "+ %s %s", $1, $3);
            free($1); free($3);
        }
    | expr '-' expr
        {
            size_t len = 2 + 1 + strlen($1) + 1 + strlen($3) + 1;
            $$ = (char*)malloc(len);
            if(!$$) { perror("malloc"); exit(1); }
            snprintf($$, len, "- %s %s", $1, $3);
            free($1); free($3);
        }
    | expr '*' expr
        {
            size_t len = 2 + 1 + strlen($1) + 1 + strlen($3) + 1;
            $$ = (char*)malloc(len);
            if(!$$) { perror("malloc"); exit(1); }
            snprintf($$, len, "* %s %s", $1, $3);
            free($1); free($3);
        }
    | expr '/' expr
        {
            size_t len = 2 + 1 + strlen($1) + 1 + strlen($3) + 1;
            $$ = (char*)malloc(len);
            if(!$$) { perror("malloc"); exit(1); }
            snprintf($$, len, "/ %s %s", $1, $3);
            free($1); free($3);
        }
    | '(' expr ')'
        {
            $$ = $2;
        }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(void) {
    return yyparse();
}

