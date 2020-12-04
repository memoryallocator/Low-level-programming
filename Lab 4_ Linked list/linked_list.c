#include "linked_list.h"

#include <stdlib.h>
#include <iso646.h>

#include "utilities.h"

static struct ListNode* list_create_new_node(const ListDataType n) {
  struct ListNode* new_node = malloc(sizeof(struct ListNode));
  if (new_node == NULL) {
    print_error_message_and_exit("failed to allocate memory");
  }

  new_node->next = NULL;
  new_node->value = n;
  return new_node;
}

struct List list_create(const ListDataType n) {
  struct List new_list;
  new_list.root = list_create_new_node(n);
  return new_list;
}

bool list_is_empty(const struct List list) {
  return list.root == NULL;
}

void list_add_front(struct List* const list_ptr, const ListDataType n) {
  struct ListNode* new_node = list_create_new_node(n);
  new_node->next = list_ptr->root;
  list_ptr->root = new_node;
}

struct ListNode* list_last_not_null_entry(const struct List list) {
  if (list_is_empty(list)) {
    return NULL;
  }

  struct ListNode* res = list.root;
  while (res->next != NULL) {
    res = res->next;
  }
  return res;
}

size_t list_length(const struct List list) {
  size_t res = 0;
  struct ListNode* curr_node = list.root;

  while (curr_node != NULL) {
    ++res;
    curr_node = curr_node->next;
  }
  return res;
}

void list_add_back(struct List* const list_ptr, const ListDataType n) {
  struct ListNode* last_not_null_entry = list_last_not_null_entry(*list_ptr);
  if (last_not_null_entry == NULL) {
    list_ptr->root = list_create_new_node(n);
    return;
  }

  last_not_null_entry->next = list_create_new_node(n);
}

void list_free(struct List list) {
  if (list_is_empty(list)) {
    return;
  }

  struct ListNode* curr_node = list.root;

  while (curr_node->next != NULL) {
    struct ListNode* next_node = curr_node->next;
    free(curr_node);
    curr_node = next_node;
  }
}

struct ListNode* list_node_at(const struct List list, const size_t idx) {
  struct ListNode* res = list.root;

  for (size_t i = 0; i < idx; ++i) {
    if (res == NULL) {
      return NULL;
    }
    res = res->next;
  }

  return res;
}

ListDataType list_get(struct List list, const size_t idx) {
  struct ListNode* res = list_node_at(list, idx);
  return res ? res->value : 0;
}

intmax_t list_sum(const struct List list) {
  intmax_t res = 0;
  struct ListNode* curr_node = list.root;

  while (curr_node != NULL) {
    res += curr_node->value;
    curr_node = curr_node->next;
  }

  return res;
}

struct List list_deep_copy(struct List list) {
  struct List res = { NULL };

  struct ListNode* curr_node = list.root;
  while (curr_node != NULL) {
    list_add_back(&res, curr_node->value);
    curr_node = curr_node->next;
  }
  return res;
}

bool list_equals(const struct List lhs, const struct List rhs) {
  // corner case: list_equals(x, x)
  if (lhs.root == rhs.root) {
    return true;
  }

  const struct ListNode* l_node = lhs.root;
  const struct ListNode* r_node = rhs.root;
  while (l_node != NULL
         and r_node != NULL) {

    if (l_node->value != r_node->value) {
      return false;
    }

    l_node = l_node->next;
    r_node = r_node->next;
  }

  return l_node->value == r_node->value;
}
