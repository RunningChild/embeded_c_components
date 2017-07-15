#ifndef __LIST_H
#define __LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct list_node
{
    void *data;
    struct list_node *next;
} list_node;


//对外函数接口
list_node* list_create(void *data);
void list_destroy(list_node **list);
list_node* list_insert_after(list_node *node, void *data);
list_node* list_insert_beginning(list_node *list, void *data);
list_node* list_insert_end(list_node *list, void *data);
void list_remove(list_node **list, list_node *node);
void list_remove_by_data(list_node **list, void *data);
list_node* list_find_node(list_node *list, list_node *node);
list_node* list_find_by_data(list_node *list, void *data);
list_node* list_find(list_node *list, int(*func)(list_node*,void*), void *data);
int list_get_len(list_node * node);
void list_display(list_node * node, void(*func)(list_node*));
list_node* list_insert_after_with_create(list_node **list, void *data);
void list_remove_with_clean(list_node **list, list_node *node, void(*func)(list_node*));
void list_destroy_with_clean(list_node **list, void(*func)(list_node*));

#ifdef __cplusplus
}
#endif

#endif
