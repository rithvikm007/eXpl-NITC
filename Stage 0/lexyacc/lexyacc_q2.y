%{
#include "y.tab.h"
%}

%%

[a-zA-Z]+ {
    yylval.str = strdup(yytext); 
    return ID;
}

[\n]      return NEWLINE;

"+"       return '+';
"-"       return '-';
"*"       return '*';
"/"       return '/';
"("       return '(';
")"       return ')';

[ \t]     ;

%%

int yywrap() {
    return 1;
}

