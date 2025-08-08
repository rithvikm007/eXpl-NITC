%option noyywrap

%{
    #include <stdlib.h>
    #include <stdio.h>
    int words = 0;
%}

%%

A[a-zA-Z]{2}    { 
	words++;
	printf("%s\n", yytext); 
}
.|\n            {}

%%

int main () {
    yylex();
    printf("Number of 3 letter words starting with A are: %d\n", words);
    return 0;
}

