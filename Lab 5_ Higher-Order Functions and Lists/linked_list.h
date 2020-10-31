#ifndef LOW_LEVEL_LINKED_LIST_H
#define LOW_LEVEL_LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef int ListDataType;

typedef struct ListNode {
    ListDataType value;
    struct ListNode* next;
} ListNode;

typedef struct {
    ListNode* root;
} List;

ListNode* list_create_node(ListDataType n);

ListNode* list_create_an_empty_node();

List list_create(ListDataType n);

void list_add_front(List* list_ptr, ListDataType n);

void list_add_back(const List* list_ptr, ListDataType n);

ListNode* list_try_advance(ListNode* node, size_t steps);

static ListNode* list_advance_unsafe(ListNode* node, size_t steps);

ListNode* list_advance(ListNode* node, size_t steps);

bool list_is_empty(List list);

ListDataType list_get(List list, size_t idx);

void list_free(List list);

void list_free_node_rec(ListNode* node);

size_t list_length(List list);

ListNode list_get_ptr_to_last_entry_and_length(List list);

ListNode* list_node_at(List list, size_t idx);

intmax_t list_sum(List list);

intmax_t list_node_sum_rec(const ListNode* node);

ListNode* list_node_deep_copy(const ListNode* node);

List list_deep_copy(List list);

bool list_equal(List lhs, List rhs);

#endif  // LOW_LEVEL_LINKED_LIST_H
