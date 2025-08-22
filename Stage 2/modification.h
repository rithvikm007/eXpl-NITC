#define PROGRAM 0
#define STATEMENT 1
#define EXPRESSION 2
#define VARIABLE 3
#define INPUT 4
#define OUTPUT 5
#define ASSIGNMENT 6
#define PLUS 7
#define MINUS 8
#define MUL 9
#define DIV 10
#define CONSTANT 11
#define INTEGER 12
#define READ 13
#define WRITE 14
#define UMINUS 15
#define UPLUS 16
typedef struct AST_Node
{
    int val;
    int type;
    char *varname;
    int nodetype;
    char op;
    char *s;
    struct AST_Node *left, *right;
} AST_Node;

struct AST_Node *makeConstantNode(int, int, char *);
struct AST_Node *makeVariableNode(int, char, char *);
struct AST_Node *makeStmtNode(int, struct AST_Node *, struct AST_Node *, char *);
struct AST_Node *makeExprNode(int, char, struct AST_Node *, struct AST_Node *, char *);

int evaluate(struct AST_Node *);
int getAddr(char *);
