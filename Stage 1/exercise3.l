%{
#include "exercise3.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>

%}

DIGIT   [0-9]
NUM     {DIGIT}+

%%

{NUM}      { yylval.integer = atoi(yytext); return NUM; }
[ \t]+     {}
"+"        { return '+'; }
"-"        { return '-'; }
"*"        { return '*'; }
"/"        { return '/'; }
"("        { return '('; }
")"        { return ')'; }
"\n"       { return '\n'; }
.          { fprintf(stderr, "Unknown char: %s\n", yytext); exit(1); }

%%
int yywrap(void){ return 1; }

