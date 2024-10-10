#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

/* Member function: Returns 1 if value is found, 0 otherwise */
int Member(int value, struct list_node_s* head_p) {
    struct list_node_s* curr_p = head_p;

    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    if (curr_p == NULL || curr_p->data > value) {
        return 0;
    } else {
        return 1;
    }
}

/* Insert function: Returns 1 if successful, 0 if value already exists */
int Insert(int value, struct list_node_s** head_pp) {
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        if (temp_p == NULL) {
            fprintf(stderr, "Malloc failed\n");
            exit(EXIT_FAILURE);
        }
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL) {
            /* New first node */
            *head_pp = temp_p;
        } else {
            pred_p->next = temp_p;
        }
        return 1;
    } else {
        /* Value already in list */
        return 0;
    }
}

/* Delete function: Returns 1 if successful, 0 if value not found */
int Delete(int value, struct list_node_s** head_pp) {
    struct list_node_s* curr_p = *head_pp;
    struct list_node_s* pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) {
            /* Deleting first node */
            *head_pp = curr_p->next;
        } else {
            /* Deleting node that is not the first */
            pred_p->next = curr_p->next;
        }
        free(curr_p);
        return 1;
    } else {
        /* Value not found */
        return 0;
    }
}

/* Check if the list is empty */
int Is_empty(struct list_node_s* head_p) {
    if (head_p == NULL)
        return 1;
    else
        return 0;
}

/* Free the linked list */
void Free_list(struct list_node_s** head_pp) {
    struct list_node_s* curr_p;
    struct list_node_s* succ_p;

    if (Is_empty(*head_pp))
        return;

    curr_p = *head_pp;
    succ_p = curr_p->next;
    while (succ_p != NULL) {
        free(curr_p);
        curr_p = succ_p;
        succ_p = curr_p->next;
    }
    free(curr_p);
    *head_pp = NULL;
}
