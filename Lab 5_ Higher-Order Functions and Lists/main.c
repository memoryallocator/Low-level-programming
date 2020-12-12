#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iso646.h>
#include <assert.h>
#include <inttypes.h>

#include "linked_list.h"
#include "list_high_order_functions.h"
#include "utilities.h"

void output_to_file(FILE* const file, const ListDataType x, const char separator) {
  fprintf(file, "%" PRId64, x);
  fprintf(file, "%c", separator);
}

void print(const ListDataType x, const char separator) {
  output_to_file(stdout, x, separator);
}

void print_with_space_as_separator(const ListDataType x) {
  print(x, ' ');
}

void print_with_newline_symbol_as_separator(const ListDataType x) {
  print(x, '\n');
}

ListDataType add(const ListDataType x, const ListDataType y) {
  return x + y;
}

ListDataType mul(const ListDataType x, const ListDataType y) {
  return x * y;
}

ListDataType mul_by_2_and_print_with_space_as_separator(const ListDataType x) {
  static bool first_run = true;
  if (first_run) {
    print_with_space_as_separator(x);
    first_run = false;
  }

  const ListDataType kXMultipliedBy2 = mul(x, 2);
  print_with_space_as_separator(kXMultipliedBy2);
  return kXMultipliedBy2;
}

ListDataType print_square_with_space_as_separator(ListDataType x) {
  const ListDataType kSquareOfX = x * x;
  print_with_space_as_separator(kSquareOfX);
  return kSquareOfX;
}

ListDataType print_cube_with_space_as_separator(ListDataType x) {
  const ListDataType kCubeOfX = x * x * x;
  print_with_space_as_separator(kCubeOfX);
  return kCubeOfX;
}

struct List read_list_from_file(FILE* const file, const bool reversed) {
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

  if (not reversed) {
    list_reverse(&res);
  }
  return res;
}

ListDataType min(const ListDataType lhs, const ListDataType rhs) {
  return lhs < rhs ? lhs : rhs;
}

ListDataType max(const ListDataType lhs, const ListDataType rhs) {
  return lhs > rhs ? lhs : rhs;
}

ListDataType get_abs_and_print_with_space_as_separator(const ListDataType x) {
  const ListDataType kAbsX = labs(x);
  print_with_space_as_separator(kAbsX);
  return kAbsX;
}

// void output_list_item_to_file_with_space_as_separator(const ListDataType x) {
//   output_to_file(output_file, x, ' ');
// }

bool save(const struct List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return false;
  }

  FILE* const output_file = fopen(filename, "w");
  if (output_file == NULL) {
    return false;
  }

  struct ListNode* curr_node = list_ptr->root;
  while (curr_node != NULL) {
    output_to_file(output_file, curr_node->value, ' ');
    curr_node = curr_node->next;
  }

  fclose(output_file);
  return true;
}

bool load(struct List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return false;
  }

  FILE* const input_file = fopen(filename, "r");
  if (input_file == NULL) {
    return false;
  }

  list_free(*list_ptr);
  *list_ptr = read_list_from_file(input_file, false);
  return true;
}

bool serialize(const struct List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return false;
  }

  FILE* const output_file = fopen(filename, "wb");
  if (output_file == NULL) {
    return false;
  }

  const size_t kListDataTypeLenInBytes = sizeof(ListDataType);
  unsigned char buffer[kListDataTypeLenInBytes];

  const struct ListNode* curr_node = list_ptr->root;
  while (curr_node != NULL) {
    const ListDataType kCurrItem = curr_node->value;
    for (size_t i = 0; i < kListDataTypeLenInBytes; ++i) {
      const ListDataType kCurrItemShifted = kCurrItem >> (8 * i);
      const unsigned char kLastByteOfShiftedValue = kCurrItemShifted & 0xff;
      buffer[i] = kLastByteOfShiftedValue;
    }

    fwrite(buffer, kListDataTypeLenInBytes, 1, output_file);
    curr_node = curr_node->next;
  }

  fclose(output_file);
  return true;
}

ListDataType buffer_to_list_data_type(const unsigned char* const buffer) {
  ListDataType res = 0;
  for (size_t i = 0; i < sizeof(ListDataType); ++i) {
    const unsigned char kIthByte = buffer[i];
    res += kIthByte << (8 * i);
  }
  return res;
}

bool deserialize(struct List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return false;
  }

  FILE* const input_file = fopen(filename, "rb");
  if (input_file == NULL) {
    return false;
  }

  const size_t kListDataTypeLenInBytes = sizeof(ListDataType);
  const unsigned char* buffer = malloc(sizeof(unsigned char) * kListDataTypeLenInBytes);

  size_t fread_returned = fread((void*) buffer, kListDataTypeLenInBytes, 1, input_file);
  if (fread_returned != 1) {
    return false;
  }

  list_free(*list_ptr);

  ListDataType value = buffer_to_list_data_type(buffer);
  *list_ptr = list_create(value);

  do {
    fread_returned = fread((void*) buffer, kListDataTypeLenInBytes, 1, input_file);
    if (fread_returned != 1) {
      break;
    }

    value = buffer_to_list_data_type(buffer);
    list_add_front(list_ptr, value);
  } while (true);

  fclose(input_file);
  list_reverse(list_ptr);
  return true;
}

int main() {
  {
    ListDataType q;
    _Static_assert(IS_INT64_T(q), "list data types other than int64_t are not supported");
  }

  struct List list = read_list_from_file(stdin, true);

  printf("The input list: ");
  foreach(list, &print_with_space_as_separator);
  printf("\n");
  printf("The input list ('\\n'-separated):\n");
  foreach(list, &print_with_newline_symbol_as_separator);
  printf("\n");

  printf("The squares of the elements of the input list: ");
  map(list, &print_square_with_space_as_separator);
  printf("\n");

  printf("The cubes of the elements of the input list: ");
  map(list, &print_cube_with_space_as_separator);
  printf("\n");

  printf("The sum of the elements of the input list: ");
  const ListDataType kSum = foldl(0, &add, list);
  printf("%" PRId64 "\n", kSum);

  printf("The minimal element of the input list: ");
  const ListDataType kMin = foldl(INT_MAX, &min, list);
  printf("%" PRId64 "\n", kMin);

  printf("The maximal element of the input list: ");
  const ListDataType kMax = foldl(INT_MIN, &max, list);
  printf("%" PRId64 "\n", kMax);

  printf("The modules of the elements of the input list: ");
  map_mut(list, get_abs_and_print_with_space_as_separator);
  printf("\n");

  const int kPowersCount = 10;
  // const int kBase = 2;
  printf("First %d powers of 2: ", kPowersCount);
  const struct List kPowersOfTwo = iterate(1, kPowersCount, mul_by_2_and_print_with_space_as_separator);
  printf("\n");

  const char* const filename = "list.txt";

  printf("Saving the input list to list.txt file...\n");
  if (not save(&list, filename)) {
    print_error_message_and_exit("save() failed");
  }

  struct List l2 = list_create(5);
  list_add_back(&l2, 3);
  list_add_front(&l2, -2);
  list_add_back(&l2, 4);

  printf("Loading the input list from list.txt file...\n");
  if (not load(&l2, filename)) {
    print_error_message_and_exit("load() failed");
  }

  printf("List contents: ");
  foreach(l2, print_with_space_as_separator);
  printf("\n");

  assert(list_equals(list, l2));

  const char* const serialized_filename = "list.bin";

  printf("Serializing...\n");
  if (not serialize(&l2, serialized_filename)) {
    print_error_message_and_exit("serialize() failed");
  }

  struct List l3 = { NULL };

  printf("Loading the input list from the serialized file...\n");
  if (not deserialize(&l3, serialized_filename)) {
    print_error_message_and_exit("deserialize() failed");
  }

  printf("List contents: ");
  foreach(l3, print_with_space_as_separator);
  printf("\n");

  assert(list_equals(l2, l3));

  list_free(list);
  list_free(l2);
  list_free(l3);
}

#pragma clang diagnostic pop