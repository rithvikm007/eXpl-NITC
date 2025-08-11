%{
#include <stdio.h>
#include <stdlib.h>
#include "exercise3.h"
#include "exercise3.c"

int yylex(void);
void yyerror(const char *s);
FILE *fptr;
%}

%union{
    tnode *node;
    int integer;
}

%token <integer> NUM
%type <node> expr
%left '+' '-'
%left '*' '/'

%%

start:
      expr '\n' {
          /* initialize registers before code generation */
          initializeRegs();

          fptr = fopen("code.xsm","w");
          if(!fptr){ perror("fopen"); exit(1); }

          /* header words */
          fprintf(fptr, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0);

          int temp = codeGen($1, fptr);

          /* move result to memory and prepare system calls */
          fprintf(fptr, "MOV [4096], R%d\n", temp);
          fprintf(fptr, "MOV R0, [4096]\n");

          /* write and exit system calls */
          fprintf(fptr,
              "MOV SP, 4095\n"
              "MOV R1, \"Write\"\n"
              "PUSH R1\n"
              "MOV R1, -2\n"
              "PUSH R1\n"
              "PUSH R0\n"
              "PUSH R1\n"
              "PUSH R1\n"
              "CALL 0\n"
              "POP R0\n"
              "POP R1\n"
              "POP R1\n"
              "POP R1\n"
              "POP R1\n"
              "MOV R1, \"Exit\"\n"
              "PUSH R1\n"
              "MOV R1, -2\n"
              "PUSH R1\n"
              "PUSH R1\n"
              "PUSH R1\n"
              "PUSH R1\n"
              "BRKP\n"
              "CALL 0\n"
          );

          fclose(fptr);
          exit(0);
      }
    ;

expr:
      '+' expr expr { $$ = makeOperatorNode('+', $2, $3); }
    | '-' expr expr { $$ = makeOperatorNode('-', $2, $3); }
    | '*' expr expr { $$ = makeOperatorNode('*', $2, $3); }
    | '/'expr expr { $$ = makeOperatorNode('/', $2, $3); }
    | '(' expr ')'  { $$ = $2; }
    | NUM           { $$ = makeLeafNode($1); }
    ;

%%

void yyerror(const char *s){
    fprintf(stderr, "yyerror: %s\n", s);
}

int main() {
    return yyparse();
}

