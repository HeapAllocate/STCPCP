#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/list.h"

typedef struct _list_element_s   
{   
    LIST_MEMBERS(struct _list_element_s);   
} list_element_s;   
   
void *list_init_element(int size)   
{   
    void *el = (void *)malloc(size);   
   
    if (!el)   
    {   
        printf("Memory allocation error!\n");   
        return NULL;   
    }   
   
    memset(el, 0, size);   
   
    return el;   
}   
   
void *list_insert_at_end(void **list, void *newel)   
{   
    list_element_s *l;   
   
    if (*list == NULL)   
    {   
        *list = newel;   
        return newel;   
    }   
       
    l = *list;   
    while (l->next != NULL)   
        l = l->next;   
   
    l->next = (list_element_s *)newel;   
       
    return newel;   
}   
   
void *list_insert_at_start(void **list, void *newel)   
{   
    ((list_element_s *)newel)->next = (list_element_s *)*list;   
    *list = newel;   
   
    return newel;   
}   
   
void *list_find_remove(void **list, void *el)   
{   
    list_element_s *l;   
   
    if (!(*list))   
        return NULL;   
   
    if (*list == el)   
    {   
        *list = ((list_element_s *)*list)->next;   
        l = ((list_element_s *)el)->next;   
        free(el);   
   
        return l;   
    }   
   
    l = *list;   
    while (l->next != (list_element_s *)el)   
    {   
        if (l->next == NULL)   
            return NULL;   
        l = l->next;   
    }   
   
    l->next = ((list_element_s *)el)->next;   
    l = l->next;   
    free(el);   
   
    return l;   
}  