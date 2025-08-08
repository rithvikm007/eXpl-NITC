/* Stop the program using Ctrl + D */
%option noyywrap

%{
    #include <stdlib.h>
    #include <stdio.h>

    int ints = 0, floats = 0;
%}

%%


[0-9]+"."[0-9]+   { floats++; }       // Float like 123.45
[0-9]+            { ints++; }         // Integer like 123

%%

int main () {
	yylex();
	printf("Number of integers: %d\n", ints);
    printf("Number of floats: %d\n", floats);
    return 0;
}
