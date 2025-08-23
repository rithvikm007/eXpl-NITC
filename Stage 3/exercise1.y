%{
	#include <stdlib.h>
	#include <stdio.h>
	#include "exercise1.h"
	#include "exercise1.c"

    extern FILE* yyin;
	extern char* yytext;

    void yyerror(char const *s);
	int yylex(void);
%}

%union{
	struct AST_Node *node;
}

%type <node> program stmt_list stmt expr InputStmt OutputStmt AsgStmt IfStmt WhileStmt ID_ NUM_
%token PLUS_ MINUS_ MUL_ DIV_ LT_ GT_ LE_ GE_ NE_ EQ_
%token BEGIN_ END_ READ_ WRITE_ IF_ THEN_ ELSE_ ENDIF_ WHILE_ DO_ ENDWHILE_
%token ID_ NUM_
%left LT_ GT_ LE_ GE_ NE_ EQ_
%left PLUS_ MINUS_
%left MUL_ DIV_

%%

program : BEGIN_ stmt_list END_ {
								$$ = $2;
								printf("Parsing Successful\n");

								evaluate($2);
								// print_tree($2, 0, 0);
								exit(1);
							}
		| BEGIN_ END_ {
			printf("Empty Program\n");
			printf("Parsing Successful\n");
			exit(1);
		}

stmt_list: stmt_list stmt ';' {$$ = makeNode(STATEMENT, VOID, $1, NULL, $2, "STATEMENT");}
	| stmt ';' {$$ = $1;}

stmt: InputStmt
    | OutputStmt
    | AsgStmt
	| IfStmt
	| WhileStmt
	
InputStmt: READ_ '(' ID_ ')' { $$ = makeNode(READ, VOID, $3, NULL, NULL, "READ");}

OutputStmt: WRITE_ '(' expr ')' { $$ = makeNode(WRITE, VOID, $3, NULL, NULL, "WRITE");}

AsgStmt: ID_ '=' expr { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "="); }

IfStmt: IF_ '(' expr ')' THEN_ stmt_list ELSE_ stmt_list ENDIF_ { $$ = makeNode(IF, VOID, $3, $6, $8, "IF");}
    | IF_ '(' expr ')' THEN_ stmt_list ENDIF_ { $$ = makeNode(IF, VOID, $3, $6, NULL, "IF");}

WhileStmt: WHILE_ '(' expr ')' DO_ stmt_list ENDWHILE_ { $$ = makeNode(WHILE, VOID, $3, NULL, $6, "WHILE");}

expr : expr PLUS_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "+");}
	| expr MINUS_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "-");}
	| expr MUL_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "*");}
	| expr DIV_ expr		{$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "/");}
	| expr LT_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<");}
	| expr GT_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">");}
	| expr LE_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<=");}
	| expr GE_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">=");}
	| expr NE_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "!=");}
	| expr EQ_ expr			{$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "==");}
	| '(' expr ')' 			{$$ = $2;}
	| NUM_					{$$ = $1;}
	| ID_					{$$ = $1;}

%%

void yyerror(char const *s)
{
    printf("yyerror %s: %s",s,yytext);
}


int main(void) 
{
    yyin=fopen("input.txt","r");
	yyparse();
	
	return 0;
}
