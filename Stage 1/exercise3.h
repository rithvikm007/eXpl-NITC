#ifndef EX2_H
#define EX2_H
typedef struct tnode{
    int val;
    char op;
    struct tnode *left,*right;
} tnode;

tnode* makeLeafNode(int n);
tnode* makeOperatorNode(char c, tnode *l, tnode *r);
int codeGen(tnode *t, FILE *target_file);

void initializeRegs(void);
int getReg(void);
void freeReg(void);
#endif
