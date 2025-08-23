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
} Nodetype;

typedef enum Type
{
    INTEGER,
    BOOLEAN,
    VOID,
} Type;

typedef struct AST_Node
{
    int val;                             // value of a number for NUM nodes.
    Type type;                           // type of variable
    char *varname;                       // name of a variable for ID nodes
    Nodetype nodetype;                   // type of node
    char *s;                             // string representation of the node
    struct AST_Node *left, *mid, *right; // left, middle and right branches
} AST_Node;

struct AST_Node *makeConstantLeafNode(Type, int, char *);
struct AST_Node *makeVariableLeafNode(Type, char, char *);
struct AST_Node *makeNode(Nodetype, Type, struct AST_Node *, struct AST_Node *, struct AST_Node *, char *);
void print_tree(struct AST_Node *, int lvl, int isRight);
