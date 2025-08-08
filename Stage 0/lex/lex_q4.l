/* Stop the program using Ctrl + D */
%option noyywrap

%{

	#include <stdio.h>
	int count = 0;

%}

lineComment  "//".*
blockComment "/*"([^*]|\*+[^*/])*\*+"/"

%%

{lineComment}   {
    count++;
    printf("Line comment: %s\n", yytext);
}

{blockComment}  {
    count++;
    printf("Block comment: %s\n", yytext);
}

.|\n    { /* Ignore everything else */ }

%%

int main() {
    yylex();
    printf("Total no of comments: %d\n", count);
    return 0;
}

