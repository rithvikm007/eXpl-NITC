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

typedef struct AST_Node
{
    int val;       // value of a number for NUM nodes.
    int type;      // type of variable
    char *varname; // name of a variable for ID nodes
    int nodetype;
    char op;                       // operator in case of operator node
    char *s;                       // string representation of the node
    struct AST_Node *left, *right; // left and right branches
} AST_Node;

//------------------- AST Functions -------------------
struct AST_Node *makeConstantNode(int, int, char *);
struct AST_Node *makeVariableNode(int, char, char *);
struct AST_Node *makeStmtNode(int, struct AST_Node *, struct AST_Node *, char *);
struct AST_Node *makeExprNode(int, char, struct AST_Node *, struct AST_Node *, char *);

//------------------- Code Gen Functions -------------------
int codeGen(struct AST_Node *, FILE *);
void readCodeGen(struct AST_Node *, FILE *);
void writeCodeGen(struct AST_Node *, FILE *);

//------------------- Reg Functions -------------------
int getReg();
void freeReg();

//------------------- Utility Functions -------------------
int getAddr(char *);
