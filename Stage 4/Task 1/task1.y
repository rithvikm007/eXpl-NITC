%{
#include <stdlib.h>
#include <stdio.h>
#include "task1.h"
#include "task1.c"

extern FILE* yyin;
extern char* yytext;
extern int yylineno;

void yyerror(char const *s);
int yylex(void);
%}

%union{
    struct AST_Node *node;
}

%type <node> program Declarations decl_list decl var_list
%type <node> stmt_list stmt InputStmt OutputStmt AsgStmt IfStmt WhileStmt RepeatUntilStmt DoWhileStmt
%type <node> BREAK_ CONTINUE_ ID_ NUM_ TEXT_ id
%type <node> expr

%token PLUS_ MINUS_ MUL_ DIV_ LT_ GT_ LE_ GE_ NE_ EQ_
%token BEGIN_ END_ READ_ WRITE_ IF_ THEN_ ELSE_ ENDIF_ WHILE_ DO_ ENDWHILE_ BREAK_ CONTINUE_ REPEAT_ UNTIL_
%token INT_ STR_ DECL_ ENDDECL_
%token ID_ NUM_ TEXT_
%left LT_ GT_ LE_ GE_ NE_ EQ_
%left PLUS_ MINUS_
%left MUL_ DIV_

%%

program : Declarations BEGIN_ stmt_list END_ {
                $$ = $3;
                GSTPrint();
                exit(0);
            }
        | Declarations BEGIN_ END_ {
            printf("Empty Program\n");
            printf("Parsing Successful\n");
            exit(1);
        };

Declarations: DECL_ decl_list ENDDECL_ {}
            | DECL_ ENDDECL_ {$$ = NULL;};

decl_list: decl_list decl
         | decl;

decl: INT_ var_list ';' {ASTChangeType($2, INTEGER);}
    | STR_ var_list ';' {ASTChangeType($2, STRING);};

var_list: var_list ',' ID_ {
                GSTInstall($3->varname, $3->type, 1);
                $$ = makeNode(STATEMENT, VOID, $1, NULL, $3, "VARLIST");
            }
        | ID_ {
                GSTInstall($1->varname, $1->type, 1);
                $$ = $1;
            };

stmt_list: stmt_list stmt ';' {$$ = makeNode(STATEMENT, VOID, $1, NULL, $2, "STATEMENT");}
         | stmt ';' {$$ = $1;};

stmt: InputStmt
    | OutputStmt
    | AsgStmt
    | IfStmt
    | WhileStmt
    | RepeatUntilStmt
    | DoWhileStmt
    | BREAK_
    | CONTINUE_;

InputStmt: READ_ '(' ID_ ')' { $$ = makeNode(READ, VOID, $3, NULL, NULL, "READ");};

OutputStmt: WRITE_ '(' expr ')' { $$ = makeNode(WRITE, VOID, $3, NULL, NULL, "WRITE");};

AsgStmt: ID_ '=' expr { $$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "="); };

IfStmt: IF_ '(' expr ')' THEN_ stmt_list ELSE_ stmt_list ENDIF_ { $$ = makeNode(IF, VOID, $3, $6, $8, "IF");}
      | IF_ '(' expr ')' THEN_ stmt_list ENDIF_ { $$ = makeNode(IF, VOID, $3, $6, NULL, "IF");};

WhileStmt: WHILE_ '(' expr ')' DO_ stmt_list ENDWHILE_ { $$ = makeNode(WHILE, VOID, $3, NULL, $6, "WHILE");};

RepeatUntilStmt: REPEAT_ stmt_list UNTIL_ '(' expr ')' { $$ = makeNode(REPEAT, VOID, $2, NULL, $5, "REPEAT"); };

DoWhileStmt: DO_ stmt_list WHILE_ '(' expr ')' { $$ = makeNode(DOWHILE, VOID, $2, NULL, $5, "DOWHILE"); };

expr : expr PLUS_ expr   {$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "+");}
     | expr MINUS_ expr  {$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "-");}
     | expr MUL_ expr    {$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "*");}
     | expr DIV_ expr    {$$ = makeNode(OPERATOR, INTEGER, $1, NULL, $3, "/");}
     | expr LT_ expr     {$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<");}
     | expr GT_ expr     {$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">");}
     | expr LE_ expr     {$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "<=");}
     | expr GE_ expr     {$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, ">=");}
     | expr NE_ expr     {$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "!=");}
     | expr EQ_ expr     {$$ = makeNode(OPERATOR, BOOLEAN, $1, NULL, $3, "==");}
     | '(' expr ')'      {$$ = $2;}
     | NUM_              {$$ = $1;}
     | id                {$$ = $1;}
     | TEXT_             {$$ = $1;};

id: ID_ {
        $$ = $1;
        struct GST_Node *curr = GSTLookup($1->varname);
        if (curr == NULL) {
            printf("Variable \"%s\" not declared\n", $1->varname);
            exit(1);
        }
        $$->type = curr->type;
    };

%%

void yyerror(char const *s) {
    printf("yyerror %s: %s", s, yytext);
}

int main(void) {
    yyin=fopen("input.txt","r");
    yyparse();
    return 0;
}
