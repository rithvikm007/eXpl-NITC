#include "task1.h"
#include <string.h>
#include <stdlib.h>

// Global Symbol Table
GST_Node *Ghead = NULL;
int binding = 4096;
int SP;

// GST Functions
GST_Node *GSTLookup(char *name)
{
    GST_Node *temp = Ghead;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

GST_Node *GSTInstall(char *name, Type type, int size)
{
    GST_Node *new_node = (GST_Node *)malloc(sizeof(GST_Node));
    new_node->name = strdup(name);
    new_node->type = type;
    new_node->size = size;
    new_node->binding = binding;
    binding += size;
    new_node->next = NULL;

    if (Ghead == NULL)
    {
        Ghead = new_node;
    }
    else
    {
        GST_Node *temp = Ghead;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new_node;
    }
    SP = binding;
    return new_node;
}

void GSTChangeType(AST_Node *root, Type type)
{
    if (root != NULL) {
        if (root->nodetype == VARIABLE) {
            GST_Node *temp = GSTLookup(root->varname);
            if (temp != NULL) {
                temp->type = type;
                root->type = type;
            }
        }
        GSTChangeType(root->left, type);
        GSTChangeType(root->right, type);
    }
}

void GSTPrint()
{
    char *type_str;
    GST_Node *temp = Ghead;
    printf("Name\tType\tSize\tBinding\n");
    while (temp != NULL)
    {
        if (temp->type == INTEGER)
            type_str = "int";
        else if (temp->type == STRING)
            type_str = "str";
        else
            type_str = "unknown";
        printf("%s\t%s\t%d\t%d\n", temp->name, type_str, temp->size, temp->binding);
        temp = temp->next;
    }
}

int getSP()
{
    return SP;
}

// AST Functions
AST_Node *makeVariableLeafNode(char *varname, char *s)
{
    AST_Node *new_node = (AST_Node *)malloc(sizeof(AST_Node));
    new_node->s = strdup(s);
    new_node->nodetype = VARIABLE;
    new_node->varname = strdup(varname);
    new_node->GSTentry = GSTLookup(varname);
    if (new_node->GSTentry)
        new_node->type = new_node->GSTentry->type;
    else
        new_node->type = VOID; // Default type until declared
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->mid = NULL;
    return new_node;
}

AST_Node *makeConstantLeafNode(Type type, int val, char *s)
{
    AST_Node *new_node = (AST_Node *)malloc(sizeof(AST_Node));
    new_node->s = strdup(s);
    new_node->nodetype = CONSTANT;
    new_node->type = type;
    new_node->val = val;
    new_node->GSTentry = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->mid = NULL;
    new_node->varname = NULL;
    return new_node;
}

AST_Node *makeNode(Nodetype node_type, Type type, AST_Node *l, AST_Node *m, AST_Node *r, char *s)
{
    AST_Node *new_node = (AST_Node *)malloc(sizeof(AST_Node));
    
    if (node_type == OPERATOR) {
        if (strcmp(s, "=") == 0) {
            if (l->GSTentry == NULL) {
                printf("Error: Left side of assignment must be a declared variable.\n");
                exit(1);
            }
            if (l->GSTentry->type != r->type) {
                printf("Error: Type mismatch in assignment. Expected type %d, got %d.\n", l->GSTentry->type, r->type);
                exit(1);
            }
        }
        else if (type == INTEGER) {
            if (l->type != INTEGER || r->type != INTEGER) {
                printf("Error: Type mismatch. Arithmetic operator requires integer operands.\n");
                exit(1);
            }
        }
        else if (type == BOOLEAN) {
             if (l->type != r->type) {
                 printf("Error: Type mismatch. Relational operator requires compatible operands.\n");
                 exit(1);
             }
        }
    }

    if (node_type == WHILE || node_type == IF) {
        if (l->type != BOOLEAN) {
            printf("Error: Type mismatch. Condition for IF/WHILE must be BOOLEAN.\n");
            exit(1);
        }
    }

    new_node->s = strdup(s);
    new_node->nodetype = node_type;
    new_node->type = type;
    new_node->left = l;
    new_node->mid = m;
    new_node->right = r;
    new_node->GSTentry = NULL;
    new_node->varname = NULL;
    return new_node;
}

struct AST_Node *ASTChangeType(struct AST_Node *root, Type type)
{
	if (root != NULL)
	{
		root->left = ASTChangeType(root->left, type);
		root->right = ASTChangeType(root->right, type);
		if (root->nodetype == VARIABLE)
		{
			GSTChangeType(root, type);
		}
		root = NULL;
		free(root);
	}
}

void printIndent(int depth, int isRight) {
    for (int i = 0; i < depth - 1; i++) {
        printf("    ");
    }
    if (depth > 0) {
        printf(isRight ? "└─ " : "├─ ");
    }
}

void print_tree(AST_Node *root, int lvl, int isRight)
{
    if (root == NULL) return;
    printIndent(lvl, isRight);
    printf("%s\n", root->s);
    if (root->left != NULL)
        print_tree(root->left, lvl + 1, 0);
    if (root->right != NULL)
        print_tree(root->right, lvl + 1, 1);
    if (root->mid != NULL)
        print_tree(root->mid, lvl + 1, 1);
}

int free_reg = -1;
int label = 0;
int u, v, inWhile = 0;

int getReg() {
    free_reg++;
    return free_reg;
}

void freeReg() {
    free_reg--;
}

int getLabel() {
    return label++;
}

int getAddr(AST_Node *t) {
    if (t->GSTentry != NULL) {
        return t->GSTentry->binding;
    }
    return -1;
}

int codeGen(AST_Node *t, FILE *target_file)
{
    int p, q, r, s, addr;

    if (t == NULL) return -1;

    if (t->nodetype == CONSTANT) {
        p = getReg();
        if (t->type == INTEGER) {
            fprintf(target_file, "MOV R%d, %d\n", p, t->val);
        } else if (t->type == STRING) {
            fprintf(target_file, "MOV R%d, %s\n", p, t->s);
        }
        return p;
    }

    if (t->nodetype == VARIABLE) {
        p = getReg();
        addr = getAddr(t);
        fprintf(target_file, "MOV R%d, [%d]\n", p, addr);
        return p;
    }

    if (t->nodetype == READ) {
        p = getReg();
        addr = getAddr(t->left);
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

    if (t->nodetype == WRITE) {
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

    if (t->nodetype == STATEMENT) {
        codeGen(t->left, target_file);
        codeGen(t->right, target_file);
        return -1;
    }

    if (t->nodetype == OPERATOR) {
        if (t->type == BOOLEAN) {
            p = codeGen(t->left, target_file);
            q = codeGen(t->right, target_file);
            if (strcmp(t->s, ">") == 0) fprintf(target_file, "GT R%d, R%d\n", p, q);
            else if (strcmp(t->s, "<") == 0) fprintf(target_file, "LT R%d, R%d\n", p, q);
            else if (strcmp(t->s, ">=") == 0) fprintf(target_file, "GE R%d, R%d\n", p, q);
            else if (strcmp(t->s, "<=") == 0) fprintf(target_file, "LE R%d, R%d\n", p, q);
            else if (strcmp(t->s, "==") == 0) fprintf(target_file, "EQ R%d, R%d\n", p, q);
            else if (strcmp(t->s, "!=") == 0) fprintf(target_file, "NE R%d, R%d\n", p, q);
            freeReg();
            return p;
        }

        if (strcmp(t->s, "=") == 0) {
            p = codeGen(t->right, target_file);
            addr = getAddr(t->left);
            fprintf(target_file, "MOV [%d], R%d\n", addr, p);
            freeReg();
            return -1;
        } else {
            p = codeGen(t->left, target_file);
            q = codeGen(t->right, target_file);
            switch (t->s[0]) {
            case '+': fprintf(target_file, "ADD R%d, R%d\n", p, q); break;
            case '-': fprintf(target_file, "SUB R%d, R%d\n", p, q); break;
            case '*': fprintf(target_file, "MUL R%d, R%d\n", p, q); break;
            case '/': fprintf(target_file, "DIV R%d, R%d\n", p, q); break;
            }
            freeReg();
            return p;
        }
    }

    else if (t->nodetype == WHILE) {
        u = getLabel();
        fprintf(target_file, "L%d:\n", u);
        p = codeGen(t->left, target_file);
        v = getLabel();
        fprintf(target_file, "JZ R%d, L%d\n", p, v);
        freeReg();
        int old_u = u, old_v = v;
        inWhile = 1;
        codeGen(t->right, target_file);
        inWhile = 0;
        u = old_u; v = old_v;
        fprintf(target_file, "JMP L%d\n", u);
        fprintf(target_file, "L%d:\n", v);
        return -1;
    }

    else if (t->nodetype == IF) {
        p = codeGen(t->left, target_file);
        s = getLabel();
        fprintf(target_file, "JZ R%d, L%d\n", p, s);
        freeReg();
        codeGen(t->mid, target_file);
        if (t->right != NULL) {
            r = getLabel();
            fprintf(target_file, "JMP L%d\n", r);
            fprintf(target_file, "L%d:\n", s);
            codeGen(t->right, target_file);
            fprintf(target_file, "L%d:\n", r);
        } else {
            fprintf(target_file, "L%d:\n", s);
        }
        return -1;
    }

    else if (t->nodetype == REPEAT) {
        u = getLabel();
        v = getLabel();
        fprintf(target_file, "L%d:\n", u);
        int old_u = u, old_v = v;
        inWhile = 1;
        codeGen(t->left, target_file);
        inWhile = 0;
        u = old_u; v = old_v;
        p = codeGen(t->right, target_file);
        fprintf(target_file, "JZ R%d, L%d\n", p, v);
        freeReg();
        fprintf(target_file, "JMP L%d\n", u);
        fprintf(target_file, "L%d:\n", v);
        return -1;
    }
    
    else if (t->nodetype == DOWHILE) {
        u = getLabel();
        v = getLabel();
        fprintf(target_file, "L%d:\n", u);
        int old_u = u, old_v = v;
        inWhile = 1;
        codeGen(t->left, target_file);
        inWhile = 0;
        u = old_u; v = old_v;
        p = codeGen(t->right, target_file);
        fprintf(target_file, "JNZ R%d, L%d\n", p, u);
        freeReg();
        fprintf(target_file, "L%d:\n", v);
        return -1;
    }

    else if (t->nodetype == BREAK) {
        if (inWhile) fprintf(target_file, "JMP L%d\n", v);
        return -1;
    }

    else if (t->nodetype == CONTINUE) {
        if (inWhile) fprintf(target_file, "JMP L%d\n", u);
        return -1;
    }
    
    return -1;
}
