#include "list.h"

#include "port.h"

/* Creates a list (node) and returns it
 * Arguments: The data the list will contain or NULL to create an empty
 * list/node
 */
list_node* list_create(void *data)
{
    list_node *l = MEM_MALLOC(sizeof(list_node));
    if (l != NULL) {
        l->next = NULL;
        l->data = data;
    }

    return l;
}

/* Completely destroys a list
 * Arguments: A pointer to a pointer to a list
 */
void list_destroy(list_node **list)
{
    if (list == NULL) return;
    while (*list != NULL) {
        list_remove(list, *list);
    }
}

/* Creates a list node and inserts it after the specified node
 * Arguments: A node to insert after and the data the new node will contain
 */
list_node* list_insert_after(list_node *node, void *data)
{
    list_node *new_node = list_create(data);
    if (new_node) {
        new_node->next = node->next;
        node->next = new_node;
    }
    return new_node;
}

/* Creates a new list node and inserts it in the beginning of the list
 * Arguments: The list the node will be inserted to and the data the node will
 * contain
 */
list_node* list_insert_beginning(list_node *list, void *data)
{
    list_node *new_node = list_create(data);
    if (new_node != NULL) { new_node->next = list; }
    return new_node;
}

/* Creates a new list node and inserts it at the end of the list
 * Arguments: The list the node will be inserted to and the data the node will
 * contain
 */
list_node* list_insert_end(list_node *list, void *data)
{
    list_node *new_node = list_create(data);
    if (new_node != NULL) {
        list_node *it;
        for(it = list; it != NULL; it = it->next) {
            if (it->next == NULL) {
                it->next = new_node;
                break;
            }
        }
    }
    return new_node;
}

/* Removes a node from the list
 * Arguments: The list and the node that will be removed
 */
void list_remove(list_node **list, list_node *node)
{
    list_node *tmp = NULL;
    if (list == NULL || *list == NULL || node == NULL) return;

    if (*list == node) {
        *list = (*list)->next;
        MEM_FREE(node);
        node = NULL;
    } else {
        tmp = *list;
        while (tmp->next && tmp->next != node) tmp = tmp->next;
        if (tmp->next) {
            tmp->next = node->next;
            MEM_FREE(node);
            node = NULL;
        }
    }
}

/* Removes an element from a list by comparing the data pointers
 * Arguments: A pointer to a pointer to a list and the pointer to the data
 */
void list_remove_by_data(list_node **list, void *data)
{
    if (list == NULL || *list == NULL || data == NULL) return;
    list_remove(list, list_find_by_data(*list, data));
}

/* Find an element in a list by the pointer to the element
 * Arguments: A pointer to a list and a pointer to the node/element
 */
list_node* list_find_node(list_node *list, list_node *node)
{
    while (list) {
        if (list == node) break;
        list = list->next;
    }
    return list;
}

/* Finds an elemt in a list by the data pointer
 * Arguments: A pointer to a list and a pointer to the data
 */
list_node* list_find_by_data(list_node *list, void *data)
{
    while (list) {
        if (list->data == data) break;
        list = list->next;
    }
    return list;
}

/* Finds an element in the list by using the comparison function
 * Arguments: A pointer to a list, the comparison function and a pointer to the
 * data
 */
list_node* list_find(list_node *list, int(*func)(list_node*,void*), void *data)
{
    if (!func) return NULL;

    while(list) {
        if (func(list, data)) break;
        list = list->next;
    }

    return list;
}

/******************************************************************************
 * @brief list_get_len
 *****************************************************************************/
int list_get_len(list_node *list)
{
    int len = 0;
    while (list) {
        len++;
        list = list->next;
    }
    return len;
}

/******************************************************************************
 * @brief list_display
 *****************************************************************************/
void list_display(list_node * list, void(*func)(list_node*))
{
    if (!func) return;
    while(list) {
        func(list);
        list = list->next;
    }
}

/******************************************************************************
 * @brief list_insert_after_with_create
 * @thread safe
 *****************************************************************************/
list_node* list_insert_after_with_create(list_node **list, void *data)
{
    //为防止中断抢占，在这里开关中断实现互斥
    ENTER_REGION_SECTION();
    if (*list == NULL)
    {
        *list = list_create(data);
    }
    else
    {
        list_insert_after(*list, data);
    }
    EXIT_REGION_SECTION();

    return *list;
}

/******************************************************************************
 * @brief list_insert_after_with_create
 * @thread safe
 *****************************************************************************/
void list_remove_with_clean(list_node **list, list_node *node, void(*func)(list_node*))
{
    list_node *tmp = NULL;
    if (list == NULL || *list == NULL || node == NULL) return;

    if (*list == node) {
        //为防止中断抢占，在这里开关中断实现互斥
        ENTER_REGION_SECTION();

        *list = (*list)->next;
        if (func)
        {
            func(node);
        }
        MEM_FREE(node);
        node = NULL;

        EXIT_REGION_SECTION();
    } else {
        tmp = *list;
        while (tmp->next && tmp->next != node) tmp = tmp->next;
        if (tmp->next) {
            //为防止中断抢占，在这里开关中断实现互斥
            ENTER_REGION_SECTION();

            tmp->next = node->next;
            if (func)
            {
                func(node);
            }
            MEM_FREE(node);
            node = NULL;

            EXIT_REGION_SECTION();
        }
    }
}

/******************************************************************************
 * @brief list_destroy_with_clean
 * @thread safe
 *****************************************************************************/
void list_destroy_with_clean(list_node **list, void(*func)(list_node*))
{
    if (list == NULL) return;
    while (*list != NULL) {
        list_remove_with_clean(list, *list, func);
    }
}

