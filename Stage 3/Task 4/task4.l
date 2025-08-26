%{
	#include <stdlib.h>
	#include <stdio.h>
	#include "y.tab.h"
	#include "task4.h"

	int number;
%}

%%

"begin" {return BEGIN_;}
"end" {return END_;}
"read" {return READ_;}
"write" {return WRITE_;}
"if" {return IF_;}
"then" {return THEN_;}
"else" {return ELSE_;}
"endif" {return ENDIF_;}
"while" {return WHILE_;}
"do" {return DO_;}
"endwhile" {return ENDWHILE_;}
"break" {yylval.node = makeNode(BREAK, VOID, NULL, NULL, NULL, "BREAK"); return BREAK_;}
"continue" {yylval.node = makeNode(CONTINUE, VOID, NULL, NULL, NULL, "CONTINUE"); return CONTINUE_;}
[a-z] {yylval.node = makeVariableLeafNode(INTEGER, *yytext, yytext); return ID_;}
[0-9]+	{number = atoi(yytext); yylval.node = makeConstantLeafNode(INTEGER, number, yytext); return NUM_;}
"+" {return PLUS_;}
"-" {return MINUS_;}
"*" {return MUL_;}
"/" {return DIV_;}
"<" {return LT_;}
"<=" {return LE_;}
">" {return GT_;}
">=" {return GE_;}
"==" {return EQ_;}
"!=" {return NE_;}
"=" {return *yytext;}
[\(\);] { return *yytext; }
[ \t\n]	{}
. {
    yyerror("unknown character\n");
    exit(1);
}

%%

int yywrap() {
	return 1;
}
