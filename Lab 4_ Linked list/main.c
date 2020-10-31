#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iso646.h>
#include <assert.h>

#include "linked_list.h"
#include "utilities.h"

List read_list_from_file(const FILE* const file) {
  List res = { NULL };

  int value;
  int fscanf_returned = fscanf(file, "%d", &value);
  if (fscanf_returned == 1) {
    res = list_create(value);

    do {
      fscanf_returned = fscanf(file, "%d", &value);
      if (fscanf_returned != 1) {
        break;
      }
      list_add_front(&res, value);
    } while (true);
  }

  return res;
}

List read_list_from_stdin() {
  return read_list_from_file(stdin);
}

size_t prompt_a_number() {
  size_t x;
  while (true) {
    printf("Your number: ");
    int scanf_returned = scanf("%lu", &x);
    printf("\n");
    if (scanf_returned != 1) {
      printf("Please try again.\n");
    } else {
      break;
    }
  }

  return x;
}

int main() {
  {
    ListDataType q;
    const bool kListDataTypeIsNotInt = _Generic(q, int: false, default: true);
    if (kListDataTypeIsNotInt) {
      print_error_message_and_exit("list data types other than int are not supported");
    }
  }

  List list = read_list_from_stdin();

  printf("The sum of the elements of the input list: ");
  const ListDataType kSum = list_sum(list);
  printf("%d\n", kSum);

  printf("Enter a number n, and I'll print the nth element of the input list!\n");
  size_t n = prompt_a_number();
  const ListNode* kNthNode = list_node_at(list, n);

  if (kNthNode == NULL) {
    print_error_message_and_exit("List is too short!\n");
  }
  printf("list[%lu] is %d\n", n, kNthNode->value);

  list_free(list);
}

#pragma clang diagnostic pop