#include <string.h>

struct AST_Node *makeVariableLeafNode(Type type, char varname, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = VARIABLE;
    new_node->type = type;
    new_node->varname = (char *)malloc(sizeof(char));
    new_node->varname[0] = varname;
    new_node->left = (struct AST_Node *)NULL;
    new_node->right = (struct AST_Node *)NULL;
    new_node->mid = (struct AST_Node *)NULL;
    return new_node;
}

struct AST_Node *makeConstantLeafNode(Type type, int val, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = CONSTANT;
    new_node->type = type;
    new_node->val = val;
    new_node->left = (struct AST_Node *)NULL;
    new_node->right = (struct AST_Node *)NULL;
    new_node->mid = (struct AST_Node *)NULL;
    new_node->varname = (char *)NULL;
    return new_node;
}

struct AST_Node *makeNode(Nodetype node_type, Type type, struct AST_Node *l, struct AST_Node *m, struct AST_Node *r, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));

    // Type checking
    if (node_type == OPERATOR && type == INTEGER)
    {
        if (l->type == BOOLEAN || r->type == BOOLEAN)
        {
            printf("Error: int mismatch 1\n");
            exit(1);
        }
    }
    if (node_type == WHILE || node_type == IF)
    {
        if (l->type == INTEGER)
        {
            printf("Error: int mismatch 2\n");
            exit(1);
        }
    }

    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = node_type;
    new_node->type = type;
    new_node->left = l;
    new_node->mid = m;
    new_node->right = r;
    new_node->varname = (char *)NULL;
    return new_node;
}

void printIndent(int depth, int isRight) {
    for (int i = 0; i < depth - 1; i++) {
        printf("    ");
    }
    if (depth > 0) {
        printf(isRight ? "└─ " : "├─ ");
    }
}

void print_tree(struct AST_Node *root, int lvl, int isRight)
{
	for(int i = 0; i < lvl; i++) printf(" ");
    if (root)
    {
    	printIndent(lvl, isRight);
        printf("%s\n", root->s);
        if (root->left != NULL)
        	print_tree(root->left, lvl + 1, 0);
        if (root->right != NULL)
        	print_tree(root->right, lvl + 1, 1);
    }
}

int free_reg = -1;

int getReg()
{
    free_reg++;
    return free_reg;
}

void freeReg()
{
    free_reg--;
}

int u, v, inWhile = 0;
int codeGen(struct AST_Node *t, FILE *target_file)
{
    int p, q, r, s, addr;

    if (t->nodetype == CONSTANT)
    {
        p = getReg();
        fprintf(target_file, "MOV R%d, %d\n", p, t->val);
        return p;
    }

    if (t->nodetype == VARIABLE)
    {
        p = getReg();
        addr = getAddr(t->varname);
        fprintf(target_file, "MOV R%d, [%d]\n", p, addr);
        return p;
    }

    if (t->nodetype == READ)
    {
        p = getReg();
        addr = getAddr(t->left->varname);
        fprintf(target_file, "MOV R%d, \"Read\"\n", p);
        fprintf(target_file, "PUSH R%d\n", p);
        fprintf(target_file, "MOV R%d, -1\n", p);
        fprintf(target_file, "PUSH R%d\n", p);
        fprintf(target_file, "MOV R%d, %d\n", p, addr);
        fprintf(target_file, "PUSH R%d\n", p);
        fprintf(target_file, "PUSH R%d\n", p);
        fprintf(target_file, "PUSH R%d\n", p);
        fprintf(target_file, "CALL 0\n");
        fprintf(target_file, "POP R%d\n", p);
        fprintf(target_file, "POP R%d\n", p);
        fprintf(target_file, "POP R%d\n", p);
        fprintf(target_file, "POP R%d\n", p);
        fprintf(target_file, "POP R%d\n", p);
        freeReg();
        return -1;
    }

    if (t->nodetype == WRITE)
    {
        p = codeGen(t->left, target_file);
        q = getReg();
        fprintf(target_file, "MOV R%d, \"Write\"\n", q);
        fprintf(target_file, "PUSH R%d\n", q);
        fprintf(target_file, "MOV R%d, -2\n", q);
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
        return -1;
    }

    if (t->nodetype == STATEMENT)
    {
        codeGen(t->left, target_file);
        codeGen(t->right, target_file);
        return -1;
    }

    if (t->nodetype == OPERATOR)
    {
        if (t->type == BOOLEAN)
        {
            p = codeGen(t->left, target_file);
            q = codeGen(t->right, target_file);
            if (strcmp(t->s, ">") == 0)
            {
                fprintf(target_file, "GT R%d, R%d\n", p, q);
            }
            else if (strcmp(t->s, "<") == 0)
            {
                fprintf(target_file, "LT R%d, R%d\n", p, q);
            }
            else if (strcmp(t->s, ">=") == 0)
            {
                fprintf(target_file, "GE R%d, R%d\n", p, q);
            }
            else if (strcmp(t->s, "<=") == 0)
            {
                fprintf(target_file, "LE R%d, R%d\n", p, q);
            }
            else if (strcmp(t->s, "==") == 0)
            {
                fprintf(target_file, "EQ R%d, R%d\n", p, q);
            }
            else if (strcmp(t->s, "!=") == 0)
            {
                fprintf(target_file, "NE R%d, R%d\n", p, q);
            }
            freeReg();
            return p;
        }

        if (t->s[0] == '=')
        {
            p = codeGen(t->right, target_file);
            addr = getAddr(t->left->varname);
            fprintf(target_file, "MOV [%d], R%d\n", addr, p);
            freeReg();
            return -1;
        }

        else
        {
            p = codeGen(t->left, target_file);
            q = codeGen(t->right, target_file);
            switch (t->s[0])
            {
            case '+':
                fprintf(target_file, "ADD R%d, R%d\n", p, q);
                freeReg();
                return p;
            case '-':
                fprintf(target_file, "SUB R%d, R%d\n", p, q);
                freeReg();
                return p;
            case '*':
                fprintf(target_file, "MUL R%d, R%d\n", p, q);
                freeReg();
                return p;
            case '/':
                fprintf(target_file, "DIV R%d, R%d\n", p, q);
                freeReg();
                return p;
            }
        }
    }

    else if (t->nodetype == WHILE)
    {
        u = getLabel();
        fprintf(target_file, "L%d:\n", u);
        p = codeGen(t->left, target_file);
        v = getLabel();
        fprintf(target_file, "JZ R%d, L%d\n", p, v);
        freeReg();
        inWhile = 1;
        p = codeGen(t->right, target_file);
        inWhile = 0;
        fprintf(target_file, "JMP L%d\n", u);
        fprintf(target_file, "L%d:\n", v);
        return p;
    }

    else if (t->nodetype == IF)
    {
        p = codeGen(t->left, target_file);
        s = getLabel();
        fprintf(target_file, "JZ R%d, L%d\n", p, s);
        freeReg();
        p = codeGen(t->mid, target_file);
        if (t->right != NULL)
        {
            r = getLabel();
            fprintf(target_file, "JMP L%d\n", r);
            fprintf(target_file, "L%d:\n", s);
            p = codeGen(t->right, target_file);
            fprintf(target_file, "L%d:\n", r);
        }
        else
        {
            fprintf(target_file, "L%d:\n", s);
        }
        return p;
    }
    else if (t->nodetype == REPEAT)
    {
        u = getLabel();
        v = getLabel();
        fprintf(target_file, "L%d:\n", u);
        inWhile = 1;
        codeGen(t->left, target_file);
        inWhile = 0;
        p = codeGen(t->right, target_file);
        fprintf(target_file, "JZ R%d, L%d\n", p, v);
        freeReg();
        fprintf(target_file, "JMP L%d\n", u);
        fprintf(target_file, "L%d:\n", v);
    }
    else if (t->nodetype == DOWHILE)
    {
        u = getLabel();
        v = getLabel();
        fprintf(target_file, "L%d:\n", u);
        inWhile = 1;
        codeGen(t->left, target_file);
        inWhile = 0;
        p = codeGen(t->right, target_file);
        fprintf(target_file, "JNZ R%d, L%d\n", p, u);
        freeReg();
        fprintf(target_file, "L%d:\n", v);
    }
    else if (t->nodetype == BREAK)
    {
        if (inWhile)
            fprintf(target_file, "JMP L%d\n", v);
    }

    else if (t->nodetype == CONTINUE)
    {
        if (inWhile)
            fprintf(target_file, "JMP L%d\n", u);
    }
}

int getAddr(char *c)
{
    return 4096 + (*c - 'a');
}

int label = 0;

int getLabel()
{
    return label++;
}
