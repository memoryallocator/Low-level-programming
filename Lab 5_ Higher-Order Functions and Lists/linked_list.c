#include "linked_list.h"

#include <stdlib.h>
#include <iso646.h>

#include "utilities.h"

ListNode* list_create_an_empty_node() {
  ListNode* new_node = ((ListNode*) calloc(sizeof(ListNode), 1));
  return new_node;
}

ListNode* list_create_node(const ListDataType n) {
  ListNode* new_node = list_create_an_empty_node();
  new_node->value = n;
  return new_node;
}

List list_create(const ListDataType n) {
  List new_list;
  new_list.root = list_create_node(n);
  return new_list;
}

bool list_is_empty(const List list) {
  return list.root == NULL;
}

ListNode* list_try_advance(ListNode* const node, const size_t steps) {
  if (node == NULL) {
    return NULL;
  }

  ListNode* res = node;
  for (size_t i = 0; i < steps; ++i) {
    res = res->next;
    if (res == NULL) {
      return res;
    }
  }
  return res;
}

static ListNode* list_advance_unsafe(ListNode* const node, const size_t steps) {
  ListNode* curr = node;
  for (size_t i = 0; i < steps; ++i) {
    curr = curr->next;
  }
  return curr;
}

ListNode* list_advance(ListNode* const node, const size_t steps) {
  ListNode* res = list_try_advance(node, steps);
  if (res == NULL) {
    print_error_message_and_exit("advance(steps, list_node) failed");
  }
  return res;
}

void list_add_front(List* const list_ptr, const ListDataType n) {
  if (list_ptr == NULL) {
    print_error_message_and_exit("list_add_front() got NULL");
  }
  ListNode* new_node = list_create_an_empty_node();
  new_node->value = n;
  new_node->next = list_ptr->root;
  list_ptr->root = new_node;
}

ListNode list_get_ptr_to_last_entry_and_length(const List list) {
  ListNode res;
  if (list_is_empty(list)) {
    res.value = 0;
    res.next = NULL;
    return res;
  }

  res.value = 1;
  res.next = list.root;
  while (true) {
    ListNode* try_advance_result = list_try_advance(res.next, 1);
    if (try_advance_result == NULL) {
      return res;
    }
    res.value++;
    res.next = try_advance_result;
  }
}

void list_add_back(const List* const list_ptr, const ListDataType n) {
  if (list_ptr == NULL) {
    print_error_message_and_exit("list_add_back() got NULL");
  }
  ListNode ptr_to_last_entry_and_length = list_get_ptr_to_last_entry_and_length(*list_ptr);
  ptr_to_last_entry_and_length.next->next = list_create_node(n);
}

void list_free_node_rec(ListNode* node) {
  if (node != NULL) {
    list_free_node_rec(node->next);
  }
  free(node);
}

void list_free(List list) {
  list_free_node_rec(list.root);
  list.root = NULL;
}

size_t list_length(const List list) {
  ListNode ptr_to_last_entry_and_length = list_get_ptr_to_last_entry_and_length(list);
  return ptr_to_last_entry_and_length.value;
}

ListNode* list_node_at(List list, const size_t idx) {
  ListNode* res = list_try_advance(list.root, idx);
  return res;
}

ListDataType list_get(List list, const size_t idx) {
  ListNode* res = list_node_at(list, idx);
  if (res) {
    return res->value;
  }
  return 0;
}

intmax_t list_node_sum_rec(const ListNode* const node) {
  if (node) {
    return node->value + list_node_sum_rec(node->next);
  }
  return 0;
}

intmax_t list_sum(const List list) {
  if (list_is_empty(list)) {
    print_error_message_and_exit("list_sum(list) got empty list as the input parameter");
  }
  return list_node_sum_rec(list.root);
}

ListNode* list_node_deep_copy(const ListNode* node) {
  ListNode* res = NULL;
  if (node != NULL) {
    res = list_create_an_empty_node();
    res->value = node->value;
    res->next = list_node_deep_copy(node->next);
  }
  return res;
}

List list_deep_copy(List list) {
  List res;
  res.root = list_node_deep_copy(list.root);
  return res;
}

bool list_equal(const List lhs, const List rhs) {
  ListNode* l_node = lhs.root;
  ListNode* r_node = rhs.root;
  while (l_node != NULL
         and r_node != NULL) {

    if (l_node->value != r_node->value) {
      return false;
    }

    l_node = l_node->next;
    r_node = r_node->next;
  }

  return l_node == r_node;
}
