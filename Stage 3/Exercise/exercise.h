#ifndef EX_H
#define EX_H

#include <stdlib.h>
#include <stdio.h>

// Enumerations for Nodetype and Type
typedef enum Nodetype
{
    VARIABLE,
    CONSTANT,
    READ,
    WRITE,
    STATEMENT,
    OPERATOR,
    WHILE,
    IF,
    BREAK,
    CONTINUE,
    REPEAT,
    DOWHILE,
} Nodetype;

typedef enum Type
{
    INTEGER,
    BOOLEAN,
    VOID,
} Type;

// AST Node Structure
typedef struct AST_Node
{
    int val;
    Type type;
    char *varname;
    Nodetype nodetype;
    char *s;
    struct AST_Node *left, *mid, *right;
} AST_Node;

// AST Node Creation Functions
struct AST_Node *makeConstantLeafNode(Type, int, char *);
struct AST_Node *makeVariableLeafNode(Type, char, char *);
struct AST_Node *makeNode(Nodetype, Type, struct AST_Node *, struct AST_Node *, struct AST_Node *, char *);

// Utility Functions
void printIndent(int, int);
void print_tree(struct AST_Node *, int, int);
int getReg();
void freeReg();
int getLabel();
int getAddr(char *);
int codeGen(struct AST_Node *, FILE *);

#endif // EX_H
