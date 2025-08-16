int free_reg = -1;

//--------------------------------AST Functions Start
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
//--------------------------------AST Functions End

//--------------------------------Reg Functions Start
int getReg()
{
    free_reg++;
    return free_reg;
}

void freeReg()
{
    free_reg--;
}
//--------------------------------Reg Functions End

//--------------------------------Code Gen Functions Start
int codeGen(struct AST_Node *t, FILE *target_file)
{
    int p = -1, q = -1, addr;
    if (t->nodetype == STATEMENT)
    {
        switch (t->type)
        {
        case STATEMENT:
            codeGen(t->left, target_file);
            codeGen(t->right, target_file);
            return -1;
        case READ:
            readCodeGen(t, target_file);
            return -1;
        case WRITE:
            writeCodeGen(t, target_file);
            return -1;
        }
    }
    else if (t->nodetype == EXPRESSION)
    {
        if (t->type == ASSIGNMENT)
        {
            p = codeGen(t->right, target_file);
            addr = getAddr(t->left->varname);
            fprintf(target_file, "MOV [%d], R%d\n", addr, p);
            return -1;
        }
        else
        {
            p = codeGen(t->left, target_file);
            q = codeGen(t->right, target_file);
            if (t->type == PLUS)
            {
                fprintf(target_file, "ADD R%d, R%d\n", p, q);
            }
            else if (t->type == MINUS)
            {
                fprintf(target_file, "SUB R%d, R%d\n", p, q);
            }
            else if (t->type == MUL)
            {
                fprintf(target_file, "MUL R%d, R%d\n", p, q);
            }
            else if (t->type == DIV)
            {
                fprintf(target_file, "DIV R%d, R%d\n", p, q);
            }
            freeReg();
            return p;
        }
    }
    else if (t->nodetype == VARIABLE)
    {
        addr = getAddr(t->varname);
        p = getReg();
        fprintf(target_file, "MOV R%d, [%d]\n", p, addr);
        return p;
    }
    else if (t->nodetype == CONSTANT)
    {
        p = getReg();
        fprintf(target_file, "MOV R%d, %d\n", p, t->val);
        return p;
    }

    return -1;
}

void readCodeGen(struct AST_Node *t, FILE *target_file)
{
    int p, q, addr;
    p = getReg();
    q = getReg();
    addr = getAddr(t->left->varname);
    fprintf(target_file, "MOV R%d,%d\n", p, addr);
    fprintf(target_file, "MOV R%d,\"Read\"\n", q);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "MOV R%d,-1\n", q);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "PUSH R%d\n", p);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "CALL 0\n");
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);
    freeReg();
    freeReg();
}

void writeCodeGen(struct AST_Node *t, FILE *target_file)
{
    int p, q, addr;
    if (t->nodetype != VARIABLE && t->nodetype != CONSTANT)
    {
        p = codeGen(t->left, target_file);
    }
    else if (t->nodetype == VARIABLE)
    {
        p = getReg();
        addr = getAddr(t->left->varname);
        fprintf(target_file, "MOV R%d, [%d]\n", p, addr);
    }
    else
    {
        p = getReg();
        fprintf(target_file, "MOV R%d, %d\n", p, t->left->val);
    }
    q = getReg();

    fprintf(target_file, "MOV R%d,\"Write\"\n", q);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "MOV R%d,-2\n", q);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "PUSH R%d\n", p);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "PUSH R%d\n", q);
    fprintf(target_file, "CALL 0\n");
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);
    fprintf(target_file, "POP R%d\n", q);

    freeReg();
    freeReg();
}

//--------------------------------Code Gen Functions End

//--------------------------------Utility Functions
int getAddr(char *c)
{
    return 4096 + (*c - 'a');
}
