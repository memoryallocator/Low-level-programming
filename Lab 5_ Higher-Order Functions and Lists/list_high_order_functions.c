#include "list_high_order_functions.h"
// #include "utilities.h"

void foreach(const List list, void (* f)(ListDataType)) {
  if (list_is_empty(list)) {
    return;
  }

  const ListNode* curr_node = list.root;
  while (curr_node != NULL) {
    f(curr_node->value);
    curr_node = list_try_advance(curr_node, 1);
  }
}

List map_mut(List list, ListDataType (* f)(ListDataType)) {
  ListNode* curr_node = list.root;
  while (curr_node != NULL) {
    curr_node->value = f(curr_node->value);
    curr_node = list_try_advance(curr_node, 1);
  }
  return list;
}

List map(List list, ListDataType (* f)(ListDataType)) {
  List mapped_list = list_deep_copy(list);
  map_mut(mapped_list, f);
  return mapped_list;
}

ListDataType foldl(ListDataType accum, ListDataType (* f)(ListDataType, ListDataType), List list) {
  if (list_is_empty(list)) {
    return accum;
  }

  ListNode* curr_node;
  ListNode* next_node = list.root;
  do {
    curr_node = next_node;
    accum = f(curr_node->value, accum);
    next_node = list_try_advance(curr_node, 1);
  } while (next_node != NULL);

  return accum;
}

List iterate(const ListDataType s, const size_t n, ListDataType (* f)(ListDataType)) {
  List res = { NULL };
  if (n == 0) {
    // print_error_message_and_exit("don't know how to iterate 0 times");
    return res;
  }

  res.root = list_create_node(s);
  ListNode* curr_node = res.root;
  for (size_t i = 0; i < n; ++i) {
    const ListDataType kNextNodeValue = f(curr_node->value);
    curr_node->next = list_create_node(kNextNodeValue);
    curr_node = curr_node->next;
  }

  return res;
}