#include <stdlib.h>
#include <stdio.h>

#include "list.h"

struct list {
    struct list_item * first;
    struct list_item * last;
    int length;
};

struct list_item {
    struct list_item * next;
    void * value;
};

List list_create(void) {
    List l = malloc(sizeof(struct list));
    l->first = NULL;
    l->last = NULL;
    l->length = 0;
    return l;
}

void list_free(List l) {
    struct list_item * curr;
    struct list_item * next;
    curr = l->first;
    printf("%p\n", curr);
    while(curr) {
        next = curr->next;
        free(curr);
        // TODO: Do I need to free the value too?
        curr = next;
    }
}

void list_append(List l, void * value) {
    struct list_item * new = (struct list_item *) malloc(sizeof(struct list_item));
    new->value = value;
    new->next = NULL;

    if(l->first == NULL) {
        // List empty
        l->first = new;
        l->last = new;
    } else {
        // List not empty
        l->last->next = new;
        l->last = new;
    }
    l->length++;
}

struct list_item * list_get_first(List l) {
    return l->first;
}

struct list_item * list_get_last(List l) {
    return l->last;
}

struct list_item * list_get_item(List l, int i) {
    struct list_item * curr;
    curr = l->first;
    int curr_idx = 0;

    while(curr && curr_idx < i) {
        curr = curr->next;
        curr_idx++;
    }

    return curr;
}


void list_for_each(List l, void (*f) (struct list_item * element, int index)) {
    struct list_item * curr;
    curr = l->first;
    int curr_idx = 0;

    while(curr) {
        (*f)(curr, curr_idx);
        curr = curr->next;
        curr_idx++;
    }
}

int list_length(List l) {
    return l->length;
}

void * list_item_value(struct list_item * i) {
    return i->value;
}

void list_item_free_value(struct list_item * el, int idx) {
    free(list_item_value(el));
}
