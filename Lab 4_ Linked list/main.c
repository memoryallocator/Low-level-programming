#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <iso646.h>

#include "linked_list.h"
#include "utilities.h"

struct List read_list_from_file(FILE* const file) {
  ListDataType value;
  int fscanf_returned = fscanf(file, "%" SCNd64, &value);
  if (fscanf_returned != 1) {
    return (struct List) { 0 };
  }

  struct List res = list_create(value);
  do {
    fscanf_returned = fscanf(file, "%" SCNd64, &value);
    if (fscanf_returned != 1) {
      break;
    }
    list_add_front(&res, value);
  } while (true);

  return res;
}

size_t prompt_a_number() {
  size_t x;
  while (true) {
    printf("Your number: ");
    int scanf_returned = scanf("%zu", &x);
    printf("\n");
    if (scanf_returned != 1) {
      while ((scanf_returned = getchar()) != EOF and scanf_returned != '\n') {
      }
      printf("Please try again.\n");
    } else {
      return x;
    }
  }
}

int main() {
  {
    ListDataType q;
    _Static_assert(IS_INT64_T(q), "list data types other than int64_t are not supported");
  }

  struct List list = read_list_from_file(stdin);

  printf("The sum of the elements of the input list: ");
  const intmax_t kSum = list_sum(list);
  printf("%jd\n", kSum);

  printf("Enter a number, and I'll print the nth element of the input list!\n");
  const size_t kN = prompt_a_number();
  const struct ListNode* kNthNode = list_node_at(list, kN);

  if (kNthNode == NULL) {
    printf("Can't print the nth element. See STDERR for more details.\n");
    fprintf(stderr, "List is too short!\n");
  } else {
    printf("list[%zu] is %" PRId64 "\n", kN, kNthNode->value);
  }

  list_reverse(&list);
  const struct List list_2 = list_deep_copy(list);
  assert(list_equals(list, list_2));
  list_free(list_2);

  list_free(list);
}

#pragma clang diagnostic pop