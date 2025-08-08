/* Stop the program using Ctrl + D */
%option noyywrap

%{
    #include <stdlib.h>
    #include <stdio.h>

    int chars = 0, words = 0, lines = 0, cur_words = 0;
%}

%%

["\n"]         { lines++, words++; cur_words = 0;}
["\t"|" "]+    { words++, cur_words++; }
.              { chars++; }
<<EOF>> {
	if(chars == 0) return 0;
	if(cur_words == 0) {
		lines++;
		return 0;
	} 
	lines++, words++; 
	return 0; 
}

%%

int main() {
    yylex();
    printf("\nNumber of words: %d\n", words);
    printf("Number of characters: %d\n", chars);
    printf("Number of lines: %d\n", lines);
    return 0;
}

