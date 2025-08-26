struct LATable
{
    char *label;
    int addr;
    struct LATable *next;
} *head, *tail;

void append(char *label, int addr)
{
    struct LATable *new_node = (struct LATable *)malloc(sizeof(struct LATable));
    new_node->label = label;
    new_node->addr = addr;
    new_node->next = NULL;
    if (head == NULL)
    {
        head = tail = new_node;
    }
    else
    {
        tail->next = new_node;
        tail = new_node;
    }
}

int getAddr(char *label)
{
    struct LATable *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->label, label) == 0)
        {
            return temp->addr;
        }
        temp = temp->next;
    }
    return -1;
}

void printTable()
{
    struct LATable *temp = head;
    while (temp != NULL)
    {
        printf("%s\t%d\n", temp->label, temp->addr);
        temp = temp->next;
    }
}