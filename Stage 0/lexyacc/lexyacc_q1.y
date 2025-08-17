%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

%union{
    char c;
}

%token <c> ID
%token NEWLINE

%left '+' '-'
%left '*' '/'

%%

input
    : 
    | input line
    ;

line
    : expr NEWLINE  { putchar('\n'); }
    ;

expr
    : expr '+' expr   { putchar('+'); }
    | expr '-' expr   { putchar('-'); }
    | expr '*' expr   { putchar('*'); }
    | expr '/' expr   { putchar('/'); }
    | '(' expr ')'    {}
    | ID              { putchar($1); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "parse error: %s\n", s);
}

int main(void) {
    return yyparse();
}

