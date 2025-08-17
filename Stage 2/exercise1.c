int variables[26];

struct AST_Node *makeVariableNode(int type, char varname, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = VARIABLE;
    new_node->type = type;
    new_node->varname = malloc(sizeof(char));
    *(new_node->varname) = varname;
    new_node->left = (struct AST_Node *)NULL;
    new_node->right = (struct AST_Node *)NULL;
    return new_node;
}

struct AST_Node *makeConstantNode(int type, int val, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = CONSTANT;
    new_node->type = type;
    new_node->val = val;
    new_node->left = (struct AST_Node *)NULL;
    new_node->right = (struct AST_Node *)NULL;
    new_node->varname = NULL;
    return new_node;
}

struct AST_Node *makeStmtNode(int type, struct AST_Node *left, struct AST_Node *right, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = STATEMENT;
    new_node->type = type;
    new_node->left = left;
    new_node->right = right;
    new_node->varname = NULL;
    return new_node;
}

struct AST_Node *makeExprNode(int type, char op, struct AST_Node *left, struct AST_Node *right, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = EXPRESSION;
    new_node->type = type;
    new_node->op = op;
    new_node->left = left;
    new_node->right = right;
    new_node->varname = NULL;
    return new_node;
}

int evaluate(struct AST_Node *t)
{
    int addr, p, q;
    if (t->nodetype == CONSTANT)
    {
        return t->val;
    }
    else if (t->nodetype == VARIABLE)
    {
        addr = getAddr(t->varname);
        return variables[addr];
    }
    else if (t->nodetype == EXPRESSION)
    {
        if (t->type == ASSIGNMENT)
        {
            p = evaluate(t->right);
            addr = getAddr(t->left->varname);
            variables[addr] = p;
            return -1;
        }
        else
        {
            p = evaluate(t->left);
            q = evaluate(t->right);
            switch (t->op)
            {
            case '+':
                return p + q;
            case '-':
                return p - q;
            case '*':
                return p * q;
            case '/':
                return p / q;
            }
        }
    }
    else if (t->nodetype == STATEMENT)
    {
        switch (t->type)
        {
        case STATEMENT:
            p = evaluate(t->left);
            q = evaluate(t->right);
            return -1;
        case READ:
            addr = getAddr(t->left->varname);
            scanf("%d", &variables[addr]);
            return -1;
        case WRITE:
            if (t->left->nodetype != VARIABLE && t->left->nodetype != CONSTANT)
            {
                p = evaluate(t->left);
                printf("%d\n", p);
            }
            else
            {
                if (t->left->nodetype == VARIABLE)
                {
                    addr = getAddr(t->left->varname);
                    printf("%d\n", variables[addr]);
                    return -1;
                }
                else
                {
                    printf("%d\n", t->left->val);
                    return -1;
                }
            }
        }
    }
}

int getAddr(char *c)
{
    return *c - 'a';
}
