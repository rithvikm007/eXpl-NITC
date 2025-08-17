%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex();
void yyerror(const char *s) { fprintf(stderr, "%s\n", s); }
%}

%union {
    char* str;
}

%token <str> ID
%left '+' '-'
%left '*' '/'
%token NEWLINE

%%

input:
  | input line
  ;

line:
    expr NEWLINE { printf("\n"); }
  ;

expr:
    expr '+' expr { printf("+ "); }
  | expr '-' expr { printf("- "); }
  | expr '*' expr { printf("* "); }
  | expr '/' expr { printf("/ "); }
  | ID           { printf("%s ", $1); free($1); }
  ;

%%

int main() {
    return yyparse();
}

