struct tnode *makeStmtNode(int type, struct tnode *left, struct tnode *right, char *s)
{
    struct tnode *new_node = (struct tnode *)malloc(sizeof(struct tnode));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = STATEMENT;
    new_node->type = type;
    new_node->left = left;
    new_node->right = right;
    new_node->varname = NULL;
    return new_node;
}

struct tnode *makeConstantNode(int type, int val, char *s)
{
    struct tnode *new_node = (struct tnode *)malloc(sizeof(struct tnode));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = CONSTANT;
    new_node->type = type;
    new_node->val = val;
    new_node->left = (struct tnode *)NULL;
    new_node->right = (struct tnode *)NULL;
    new_node->varname = NULL;
    return new_node;
}

struct tnode *makeVariableNode(int type, char varname, char *s)
{
    struct tnode *new_node = (struct tnode *)malloc(sizeof(struct tnode));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = VARIABLE;
    new_node->type = type;
    new_node->varname = malloc(sizeof(char));
    *(new_node->varname) = varname;
    new_node->left = (struct tnode *)NULL;
    new_node->right = (struct tnode *)NULL;
    return new_node;
}

struct tnode *makeExprNode(int type, char op, struct tnode *left, struct tnode *right, char *s)
{
    struct tnode *new_node = (struct tnode *)malloc(sizeof(struct tnode));
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

void printIndent(int depth, int isRight) {
    for (int i = 0; i < depth - 1; i++) {
        printf("    ");
    }
    if (depth > 0) {
        printf(isRight ? "└─ " : "├─ ");
    }
}

void print_tree(struct tnode *root, int lvl, int isRight)
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
