#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* Node structure for the linked list */
struct list_node_s {
    int data;
    struct list_node_s* next;
};

/* Function prototypes */
int Member(int value, struct list_node_s* head_p);
int Insert(int value, struct list_node_s** head_pp);
int Delete(int value, struct list_node_s** head_pp);
void Free_list(struct list_node_s** head_pp);
int Is_empty(struct list_node_s* head_p);

#endif
