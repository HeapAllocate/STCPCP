#ifndef _LIST_H_
#define _LIST_H_

#define LIST_MEMBERS(a) a *next

void *list_init_element(int size); 
void *list_insert_at_end(void **list, void *newel); 
void *list_insert_at_start(void **list, void *newel); 
void *list_find_remove(void **list, void *el);

#endif