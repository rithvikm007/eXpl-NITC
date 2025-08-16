%{
	#include <stdlib.h>
	#include <stdio.h>
	#include "task2.h"
	#include "task2.c"

    extern FILE* yyin;

    void yyerror(char const *s);
	int yylex(void);
%}

%union{
	struct AST_Node *node;
}

%type <node> program stmt_list stmt expr _ID _NUM
%token _PLUS _MINUS _MUL _DIV
%token _BEGIN _END _READ _WRITE _ID _NUM
%left _PLUS _MINUS
%left _MUL _DIV

%%

program : _BEGIN stmt_list _END ';' {
								$$ = $2;
								
								FILE *fp = fopen("code.xsm", "w");
								if (fp == NULL)
								{
									printf("Error opening file!\n");
									exit(1);
								}

								fprintf(fp, "0\n2056\n0\n0\n0\n0\n0\n0\n");
								fprintf(fp, "MOV SP, 4122\n");

								codeGen($2, fp);

								fprintf(fp, "INT 10\n");

								printf("Code Generated\n");
								exit(0);
							}
		| _BEGIN _END ';' {
			printf("Empty Program\n");
			printf("Parsing Successful\n");
			exit(0);
		}

stmt_list: stmt_list stmt ';' {$$ = makeStmtNode(STATEMENT, $1, $2, "STATEMENT");}
	| stmt ';' {$$ = $1;}

stmt : _READ '(' _ID ')' { $$ = makeStmtNode(READ, $3, (struct AST_Node *)NULL, "READ");}
    | _WRITE '(' expr ')' { $$ = makeStmtNode(WRITE, $3, (struct AST_Node *)NULL, "WRITE");}
    | _ID '=' expr { $$ = makeExprNode(ASSIGNMENT, '=', $1, $3, "="); }
	
expr : expr _PLUS expr		{$$ = makeExprNode(PLUS, '+',$1, $3, "+");}
	| expr _MINUS expr  	{$$ = makeExprNode(MINUS, '-',$1, $3, "-");}
	| expr _MUL expr	{$$ = makeExprNode(MUL, '*',$1, $3, "*");}
	| expr _DIV expr	{$$ = makeExprNode(DIV, '/',$1, $3, "/");}
	| '(' expr ')' 	{$$ = $2;}
	| _NUM		{$$ = $1;}
	| _ID		{$$ = $1;}

%%

void yyerror(char const *s)
{
    printf("yyerror %s",s);
}


int main() 
{
    yyin=fopen("input.txt","r");
	yyparse();
	
	return 0;
}
