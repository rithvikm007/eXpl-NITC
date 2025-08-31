#ifndef EX1_H
#define EX1_H

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
    ARRAY,
} Nodetype;

typedef enum Type
{
    INTEGER,
    BOOLEAN,
    VOID,
    STRING,
} Type;

// Forward declaration
struct GST_Node;

// AST Node Structure
typedef struct AST_Node
{
    int val;
    Type type;
    char *varname;
    Nodetype nodetype;
    struct GST_Node *GSTentry;
    char *s;
    struct AST_Node *left, *mid, *right;
} AST_Node;

// GST Node Structure
typedef struct GST_Node
{
    char *name;
    int type;
    int size;
    int size2;
    int binding;
    int dimensions;
    struct GST_Node *next;
} GST_Node;

// AST Node Creation Functions
struct AST_Node *makeConstantLeafNode(Type, int, char *);
struct AST_Node *makeVariableLeafNode(char *, char *);
struct AST_Node *makeNode(Nodetype, Type, struct AST_Node *, struct AST_Node *, struct AST_Node *, char *);
struct AST_Node *makeArrayLeafNode(char *, struct AST_Node *, char *);

// GST Related Functions
struct GST_Node *GSTLookup(char *);
struct GST_Node *GSTInstall(char *, Type, int size, int size2, int dimensions);
void GSTChangeType(struct AST_Node *, Type);
void GSTPrint();

// Utility Functions
void printIndent(int, int);
void print_tree(struct AST_Node *, int, int);
int getReg();
void freeReg();
int getLabel();
int getAddr(struct AST_Node *t);
int codeGen(struct AST_Node *, FILE *);

#endif // EX1_H
