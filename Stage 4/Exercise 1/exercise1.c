#include "exercise1.h"
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

GST_Node *GSTInstall(char *name, Type type, int size1, int size2, int dimensions)
{
    GST_Node *new_node = (GST_Node *)malloc(sizeof(GST_Node));
    new_node->name = strdup(name);
    new_node->type = type;
    new_node->size = size1;
    new_node->size2 = size2;
    new_node->dimensions = dimensions;
    new_node->binding = binding;

    int total_size = size1 * size2;
    if (dimensions == 1) total_size = size1;
    if (dimensions == 0) total_size = 1;

    binding += total_size;
    new_node->next = NULL;

    if (Ghead == NULL) {
        Ghead = new_node;
    } else {
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
    char *type;
    char *array;
    struct GST_Node *temp = Ghead;
    printf("Name\tType\tSize\tArray\tBinding\n");
    while (temp != NULL)
    {
        if (temp->type == 0)
        {
            type = (char *)malloc(sizeof(char) * 4);
            strcpy(type, "int");
        }
        else if (temp->type == 2)
        {
            type = (char *)malloc(sizeof(char) * 4);
            strcpy(type, "str");
        }

        if (temp->dimensions == 0)
        {
            array = (char *)malloc(sizeof(char) * 3);
            strcpy(array, "no");
        }
        else if (temp->dimensions == 1 || temp->dimensions == 2)
        {
            array = (char *)malloc(sizeof(char) * 4);
            strcpy(array, "yes");
        }

        printf("%s\t%s\t%d\t%s\t%d\n", temp->name, type, temp->size * temp->size2, array, temp->binding);
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

struct AST_Node *makeArrayLeafNode(char *varname, struct AST_Node *l, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = (char *)malloc(sizeof(char) * strlen(s));
    new_node->s = strdup(s);
    new_node->nodetype = ARRAY;
    new_node->varname = (char *)malloc(sizeof(char) * strlen(varname));
    new_node->varname = strdup(varname);
    new_node->GSTentry = GSTLookup(varname);
    new_node->type = new_node->GSTentry->type;
    new_node->left = l;
    new_node->right = (struct AST_Node *)NULL;
    new_node->mid = (struct AST_Node *)NULL;
    return new_node;
}

struct AST_Node *makeArray2DLeafNode(char *varname, struct AST_Node *row, struct AST_Node *col, char *s)
{
    struct AST_Node *new_node = (struct AST_Node *)malloc(sizeof(struct AST_Node));
    new_node->s = strdup(s);
    new_node->nodetype = ARRAY;
    new_node->varname = strdup(varname);
    new_node->GSTentry = GSTLookup(varname);
    new_node->type = new_node->GSTentry->type;
    new_node->left = row; 
    new_node->mid = col;
    new_node->right = NULL;
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
        printf("|   ");
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
		int p = getReg();
		int q = getReg();
		int addr;

		if (t->left->nodetype == ARRAY) {
		    if (t->left->GSTentry->dimensions == 2) {
		        addr = getAddr(t->left);
		        int row_reg = codeGen(t->left->left, target_file);
		        int col_reg = codeGen(t->left->mid, target_file);
		        
		        int dim_reg = getReg();
		        fprintf(target_file, "MOV R%d, %d\n", dim_reg, t->left->GSTentry->size2);
		        fprintf(target_file, "MUL R%d, R%d\n", row_reg, dim_reg);
		        fprintf(target_file, "ADD R%d, R%d\n", row_reg, col_reg);

		        fprintf(target_file, "MOV R%d, %d\n", p, addr);
		        fprintf(target_file, "ADD R%d, R%d\n", p, row_reg);
		        
		        freeReg(); // col_reg
		        freeReg(); // dim_reg
		        freeReg(); // row_reg
		    } else {
		        addr = getAddr(t->left);
		        q = codeGen(t->left->left, target_file);
		        fprintf(target_file, "MOV R%d, %d\n", p, addr);
		        fprintf(target_file, "ADD R%d, R%d\n", p, q);
		        freeReg();
		    }
		} else {
		    addr = getAddr(t->left);
		    fprintf(target_file, "MOV R%d, %d\n", p, addr);
		}
		
		// System call to read from the calculated address
		fprintf(target_file, "MOV R%d, \"Read\"\n", q);
		fprintf(target_file, "PUSH R%d\n", q);
		fprintf(target_file, "MOV R%d, -1\n", q);
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

    if (t->nodetype == WRITE) {
		int p;
		int q = getReg();

		if (t->left->nodetype == ARRAY) {
		    int addr = getAddr(t->left);
		    if (t->left->GSTentry->dimensions == 2) {
		        int row_reg = codeGen(t->left->left, target_file);
		        int col_reg = codeGen(t->left->mid, target_file);
		        int dim_reg = getReg();
		        int addr_reg = getReg();

		        fprintf(target_file, "MOV R%d, %d\n", dim_reg, t->left->GSTentry->size2);
		        fprintf(target_file, "MUL R%d, R%d\n", row_reg, dim_reg);
		        fprintf(target_file, "ADD R%d, R%d\n", row_reg, col_reg);

		        fprintf(target_file, "MOV R%d, %d\n", addr_reg, addr);
		        fprintf(target_file, "ADD R%d, R%d\n", addr_reg, row_reg);

		        p = getReg();
		        fprintf(target_file, "MOV R%d, [R%d]\n", p, addr_reg);

		        freeReg(); // addr_reg
		        freeReg(); // dim_reg
		        freeReg(); // col_reg
		        freeReg(); // row_reg
		    } else {
		        p = getReg();
		        int index_reg = codeGen(t->left->left, target_file);
		        fprintf(target_file, "MOV R%d, %d\n", p, addr);
		        fprintf(target_file, "ADD R%d, R%d\n", p, index_reg);
		        fprintf(target_file, "MOV R%d, [R%d]\n", p, p);
		        freeReg();
		    }
		} else {
		    p = codeGen(t->left, target_file);
		}

		// System call to write the value
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
			int p = codeGen(t->right, target_file);
			int addr;

			if (t->left->nodetype == ARRAY) {
				if (t->left->GSTentry->dimensions == 2) {
					int row_reg = codeGen(t->left->left, target_file);
					int col_reg = codeGen(t->left->mid, target_file);
					int dim_reg = getReg();
					int addr_reg = getReg();
					
					fprintf(target_file, "MOV R%d, %d\n", dim_reg, t->left->GSTentry->size2);
					fprintf(target_file, "MUL R%d, R%d\n", row_reg, dim_reg);
					fprintf(target_file, "ADD R%d, R%d\n", row_reg, col_reg);

					addr = getAddr(t->left);
					fprintf(target_file, "MOV R%d, %d\n", addr_reg, addr);
					fprintf(target_file, "ADD R%d, R%d\n", addr_reg, row_reg);

					fprintf(target_file, "MOV [R%d], R%d\n", addr_reg, p);
					
					freeReg(); 
					freeReg();
					freeReg();
					freeReg(); 
				} else {
					int addr_reg = getReg();
					int index_reg = codeGen(t->left->left, target_file);
					addr = getAddr(t->left);
					fprintf(target_file, "MOV R%d, %d\n", addr_reg, addr);
					fprintf(target_file, "ADD R%d, R%d\n", addr_reg, index_reg);
					fprintf(target_file, "MOV [R%d], R%d\n", addr_reg, p);
					freeReg();
					freeReg();
				}
				freeReg();
				return -1;
			} else {
				addr = getAddr(t->left);
				fprintf(target_file, "MOV [%d], R%d\n", addr, p);
				freeReg();
				return -1;
			}
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
    
    else if (t->nodetype == ARRAY)
    {
    	if (t->GSTentry->dimensions == 2)
		{
		    addr = getAddr(t);
		    p = codeGen(t->left, target_file);
		    q = codeGen(t->mid, target_file);

		    int dim_reg = getReg(); 
		    int offset_reg = getReg();

		    // Assuming your `GST_Node` now stores dimensions, e.g., `t->GSTentry->dimensions`
		    // You need to find a way to store the actual size of the second dimension (columns)
		    // You could store this in the GST_Node or pass it.
		    // For this exercise, let's assume `n` is a constant. For a full compiler, you'd need to store the size in the symbol table.

		    // Let's assume the size of the second dimension is stored in `t->GSTentry->size2`
		    // You need to add `size2` to the `GST_Node` and populate it in `GSTInstall`
		    // To be simple, let's just assume it's `10` from the example `a[10][10]`
		    int n = t->GSTentry->size2;

		    // Calculate the linear offset: `(row * n) + col`
		    fprintf(target_file, "MOV R%d, %d\n", dim_reg, n); // Load the dimension size
		    fprintf(target_file, "MUL R%d, R%d\n", p, dim_reg); // p = row * n
		    fprintf(target_file, "ADD R%d, R%d\n", p, q); // p = (row * n) + col

		    // Now, `p` holds the offset. Add this to the base address
		    fprintf(target_file, "MOV R%d, %d\n", offset_reg, addr);
		    fprintf(target_file, "ADD R%d, R%d\n", offset_reg, p);

		    // Dereference the final address to get the value
		    fprintf(target_file, "MOV R%d, [R%d]\n", p, offset_reg);

		    // Free the registers
		    freeReg(); // offset_reg
		    freeReg(); // dim_reg
		    freeReg(); // q

		    return p;
		}
		else {		
		    addr = getAddr(t);
		    p = codeGen(t->left, target_file);
		    q = getReg();
		    fprintf(target_file, "MOV R%d, %d\n", q, addr);
		    fprintf(target_file, "ADD R%d, R%d\n", q, p);
		    fprintf(target_file, "MOV R%d, [R%d]\n", p, q);
		    freeReg();
	        return p;
		}
    }
    
    return -1;
}
