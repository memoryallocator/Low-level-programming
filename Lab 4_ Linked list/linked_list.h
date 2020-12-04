#ifndef LOW_LEVEL_LINKED_LIST_H
#define LOW_LEVEL_LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef int64_t ListDataType;

struct ListNode {
    ListDataType value;
    struct ListNode* next;
};

struct List {
    struct ListNode* root;
};

struct List list_create(ListDataType n);

void list_add_front(struct List* list_ptr, ListDataType n);

void list_add_back(struct List* list_ptr, ListDataType n);

bool list_is_empty(struct List list);

ListDataType list_get(struct List list, size_t idx);

void list_free(struct List list);

struct ListNode* list_last_not_null_entry(struct List list);

size_t list_length(struct List list);

struct ListNode* list_node_at(struct List list, size_t idx);

intmax_t list_sum(struct List list);

struct List list_deep_copy(struct List list);

bool list_equals(struct List lhs, struct List rhs);

#endif  // LOW_LEVEL_LINKED_LIST_H
