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

typedef struct tnode
{
    int val;       // value of a number for NUM nodes.
    int type;      // type of variable
    char *varname; // name of a variable for ID nodes
    int nodetype;
    char op;                       // operator in case of operator node
    char *s;                       // string representation of the node
    struct AST_Node *left, *right; // left and right branches
} tnode;

struct tnode *makeConstantNode(int, int, char *);
struct tnode *makeVariableNode(int, char, char *);
struct tnode *makeStmtNode(int, struct tnode *, struct tnode *, char *);
struct tnode *makeExprNode(int, char, struct tnode *, struct tnode *, char *);
void print_tree(struct tnode *, int lvl, int isRight);
