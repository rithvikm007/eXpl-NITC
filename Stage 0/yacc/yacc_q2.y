%{
#include <stdio.h>
#include <ctype.h>

void yyerror(const char *s);
int yylex(void);

char current_id[100];  // buffer for current identifier
int id_index = 0;
%}

%token LETTER DIGIT INVALID

%%

input:
    variable '\n' {
        current_id[id_index] = '\0';
        printf("Valid variable name: %s\n", current_id);
        return 0;
    }
  | '\n' {
        yyerror("No input given");
        return 0;
    }
  | INVALID {
        yyerror("Invalid variable name");
        return 0;
    }
  ;

variable:
    LETTER {
        current_id[id_index++] = yylval;
    }
    tail
  ;

tail:
    /* empty */
  | tail_char tail
  ;

tail_char:
    LETTER {
        current_id[id_index++] = yylval;
    }
  | DIGIT {
        current_id[id_index++] = yylval;
    }
  ;

%%

int yylex(void) {
    int c;

    // Skip whitespace
    while (isspace(c = getchar())) {
        if (c == '\n') return '\n';
    }

    if (isalpha(c)) {
        yylval = c;
        return LETTER;
    }

    if (isdigit(c)) {
        yylval = c;
        return DIGIT;
    }

    if (c == EOF)
        return 0;

    return INVALID;
}

void yyerror(const char *s) {
    printf("Invalid variable name\n");
}

int main(void) {
    printf("Enter variable name:\n");
    yyparse();
    return 0;
}

