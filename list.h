#ifndef MM_LIST_H
#define MM_LIST_H

struct list;
struct list_item;

typedef struct list * List;

List list_create(void);
void list_free(List l);
void list_append(List l, void * value);
struct list_item * list_get_item(List l, int i);
struct list_item * list_get_first(List l);
struct list_item * list_get_last(List l);
void list_for_each(List l, void (*f) (struct list_item * element, int index));
int list_length(List l);
void * list_item_value(struct list_item * i);
void list_item_free_value(struct list_item * el, int idx);

#endif
