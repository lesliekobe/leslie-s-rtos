#ifndef LIST_H
#define LIST_H

#include "types.h"

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

typedef struct {
    list_node_t head;
    u32 count;
} list_t;

static inline void list_init(list_t *lst) {
    lst->head.next = &lst->head;
    lst->head.prev = &lst->head;
    lst->count = 0;
}

static inline void list_add(list_t *lst, list_node_t *node) {
    node->next = &lst->head;
    node->prev = lst->head.prev;
    lst->head.prev->next = node;
    lst->head.prev = node;
    lst->count++;
}

static inline void list_add_tail(list_t *lst, list_node_t *node) {
    node->next = &lst->head;
    node->prev = lst->head.prev;
    lst->head.prev->next = node;
    lst->head.prev = node;
    lst->count++;
}

static inline void list_remove(list_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node->prev = NULL;
}

static inline u32 list_count(list_t *lst) { return lst->count; }
static inline int list_empty(list_t *lst) { return lst->count == 0; }

static inline list_node_t* list_pop(list_t *lst) {
    if (lst->count == 0) return NULL;
    list_node_t *n = lst->head.next;
    list_remove(n);
    lst->count--;
    return n;
}

#define list_foreach(lst, type, member, iter) \
    for (iter = (type*)((char*)((lst)->head.next) - offsetof(type, member)); \
         &((iter)->member) != &(lst)->head; \
         iter = (type*)((char*)((iter)->member.next) - offsetof(type, member)))

#define list_foreach_safe(lst, type, member, iter, tmp) \
    for (iter = (type*)((char*)((lst)->head.next) - offsetof(type, member)); \
         &((iter)->member) != &(lst)->head; \
         iter = (type*)((char*)((tmp = (iter)->member.next) ? tmp - offsetof(type, member) : tmp)))

#endif /* LIST_H */