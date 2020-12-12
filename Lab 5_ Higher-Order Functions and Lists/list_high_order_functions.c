#include "list_high_order_functions.h"
// #include "utilities.h"

void foreach(const struct List list, void (* f)(ListDataType)) {
  const struct ListNode* curr_node = list.root;
  while (curr_node != NULL) {
    f(curr_node->value);
    curr_node = curr_node->next;
  }
}

struct List map_mut(struct List list, ListDataType (* f)(ListDataType)) {
  struct ListNode* curr_node = list.root;
  while (curr_node != NULL) {
    curr_node->value = f(curr_node->value);
    curr_node = curr_node->next;
  }
  return list;
}

struct List map(struct List list, ListDataType (* f)(ListDataType)) {
  struct List mapped_list = list_deep_copy(list);
  map_mut(mapped_list, f);
  return mapped_list;
}

ListDataType foldl(ListDataType accum, ListDataType (* f)(ListDataType, ListDataType), struct List list) {
  struct ListNode* curr_node = list.root;
  while (curr_node != NULL) {
    accum = f(curr_node->value, accum);
    curr_node = curr_node->next;
  }
  return accum;
}

struct List iterate(const ListDataType s, const size_t n, ListDataType (* f)(ListDataType)) {
  if (n == 0) {
    // print_error_message_and_exit("don't know how to iterate 0 times");
    return (struct List) { NULL };
  }

  struct List res = list_create(s);
  const struct ListNode* curr_node = res.root;
  for (size_t i = 0; i < n; ++i) {
    const ListDataType kNextNodeValue = f(curr_node->value);
    list_add_front(&res, kNextNodeValue);
    curr_node = res.root;
  }

  list_reverse(&res);
  return res;
}