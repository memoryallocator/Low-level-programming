#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iso646.h>
#include <assert.h>

#include "linked_list.h"
#include "list_high_order_functions.h"
#include "utilities.h"

void output_to_file(const FILE* const file, const ListDataType x, const char separator) {
  fprintf(file, "%d%c", x, separator);
}

void print(const ListDataType x, const char separator) {
  output_to_file(stdout, x, separator);
}

void print_with_space_as_separator(const ListDataType x) {
  return print(x, ' ');
}

void print_with_newline_symbol_as_separator(const ListDataType x) {
  return print(x, '\n');
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
  ListDataType kSquareOfX = x * x;
  print_with_space_as_separator(kSquareOfX);
  return kSquareOfX;
}

ListDataType print_cube_with_space_as_separator(ListDataType x) {
  ListDataType kCubeOfX = x * x * x;
  print_with_space_as_separator(kCubeOfX);
  return kCubeOfX;
}

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

ListDataType min(const ListDataType lhs, const ListDataType rhs) {
  return lhs < rhs ? lhs : rhs;
}

ListDataType max(const ListDataType lhs, const ListDataType rhs) {
  return lhs > rhs ? lhs : rhs;
}

ListDataType get_abs_and_print_with_space_as_separator(const ListDataType x) {
  const ListDataType kAbsX = abs(x);
  print_with_space_as_separator(kAbsX);
  return kAbsX;
}

const FILE* output_file;

void output_list_item_to_file_with_space_as_separator(const ListDataType x) {
  output_to_file(output_file, x, ' ');
}

bool file_exists(const char* const filename) {
  output_file = fopen(filename, "r");
  if (output_file != NULL) {
    // file already exists!
    fclose(output_file);
    return true;
  }
  return false;
}

bool save(const List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return 0;
  }

  if (file_exists(filename)) {
    return false;
  }

  output_file = fopen(filename, "w");
  if (output_file == NULL) {
    return false;
  }

  foreach(*list_ptr, output_list_item_to_file_with_space_as_separator);
  fclose(output_file);
  return true;
}

bool load(List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return 0;
  }

  if (not file_exists(filename)) {
    return false;
  }

  const FILE* const input_file = fopen(filename, "r");
  if (input_file == NULL) {
    return false;
  }

  int value;
  int fscanf_returned = fscanf(input_file, "%d", &value);
  if (fscanf_returned != 1) {
    list_free(*list_ptr);
    list_ptr->root = NULL;
  } else {
    if (list_ptr->root != NULL) {
      list_ptr->root->value = value;
    } else {
      list_ptr->root = list_create_node(value);
    }

    ListNode** last_valid_node_ptr = &(list_ptr->root);
    ListNode** ptr_to_next_node = &(list_ptr->root->next);
    do {
      fscanf_returned = fscanf(input_file, "%d", &value);
      if (fscanf_returned != 1) {
        break;
      }

      if (*ptr_to_next_node == NULL) {
        *ptr_to_next_node = list_create_node(value);
      } else {
        (*ptr_to_next_node)->value = value;
      }

      last_valid_node_ptr = ptr_to_next_node;
      ptr_to_next_node = &((*ptr_to_next_node)->next);
    } while (true);

    list_free_node_rec(*ptr_to_next_node);
    (*last_valid_node_ptr)->next = NULL;
  }

  return true;
}

bool serialize(const List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return 0;
  }

  if (file_exists(filename)) {
    return false;
  }

  output_file = fopen(filename, "wb");
  if (output_file == NULL) {
    return false;
  }

  const size_t kListDataTypeLenInBytes = sizeof(ListDataType);
  unsigned char buffer[kListDataTypeLenInBytes];

  const ListNode* curr_node = list_ptr->root;
  while (curr_node != NULL) {
    const int kCurrItem = curr_node->value;
    for (size_t i = 0; i < kListDataTypeLenInBytes; ++i) {
      buffer[i] = ((char) (kCurrItem >> (8 * i) & 0xff));
    }

    fwrite(buffer, kListDataTypeLenInBytes, 1, output_file);
    curr_node = curr_node->next;
  }

  fclose(output_file);
  return true;
}

int four_bytes_buffer_to_int(const unsigned char* const buffer) {
  int value = 0;
  for (size_t i = 0; i < 4; ++i) {
    const unsigned char kIthByte = buffer[i];
    value += kIthByte << (8 * i);
  }
  return value;
}

bool deserialize(List* const list_ptr, const char* const filename) {
  if (list_ptr == NULL) {
    return 0;
  }

  if (not file_exists(filename)) {
    return false;
  }

  FILE* input_file = fopen(filename, "rb");
  if (input_file == NULL) {
    return false;
  }

  const size_t kListDataTypeLenInBytes = sizeof(ListDataType);
  const unsigned char* buffer = ((unsigned char*) malloc(sizeof(unsigned char) * kListDataTypeLenInBytes));
  int fread_returned = fread(buffer, kListDataTypeLenInBytes, 1, input_file);
  if (fread_returned != 1) {
    list_free(*list_ptr);
    list_ptr->root = NULL;
  } else {
    ListDataType value = four_bytes_buffer_to_int(buffer);

    if (list_ptr->root != NULL) {
      list_ptr->root->value = value;
    } else {
      list_ptr->root = list_create_node(value);
    }

    ListNode** last_valid_node_ptr = &(list_ptr->root);
    ListNode** ptr_to_next_node = &(list_ptr->root->next);
    do {
      fread_returned = fread(buffer, kListDataTypeLenInBytes, 1, input_file);
      if (fread_returned != 1) {
        break;
      }

      value = four_bytes_buffer_to_int(buffer);
      if (*ptr_to_next_node == NULL) {
        *ptr_to_next_node = list_create_node(value);
      } else {
        (*ptr_to_next_node)->value = value;
      }

      last_valid_node_ptr = ptr_to_next_node;
      ptr_to_next_node = &((*ptr_to_next_node)->next);
    } while (true);

    list_free_node_rec(*ptr_to_next_node);
    (*last_valid_node_ptr)->next = NULL;
  }

  fclose(input_file);
  return true;
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
  printf("%d\n", kSum);

  printf("The minimal element of the input list: ");
  const ListDataType kMin = foldl(INT_MAX, &min, list);
  printf("%d\n", kMin);

  printf("The maximal element of the input list: ");
  const ListDataType kMax = foldl(INT_MIN, &max, list);
  printf("%d\n", kMax);

  printf("The modules of the elements of the input list: ");
  map_mut(list, get_abs_and_print_with_space_as_separator);
  printf("\n");

  const int kPowersCount = 10;
  // const int kBase = 2;
  printf("First %d powers of 2: ", kPowersCount);
  const List kPowersOfTwo = iterate(1, kPowersCount, mul_by_2_and_print_with_space_as_separator);
  printf("\n");

  const char* const filename = "list.txt";

  printf("Saving the input list to list.txt file...\n");
  if (not save(&list, filename)) {
    print_error_message_and_exit("save() failed");
  }

  List l2 = list_create(5);
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

  assert(list_equal(list, l2));

  const char* const serialized_filename = "list.bin";

  printf("Serializing...\n");
  if (not serialize(&l2, serialized_filename)) {
    print_error_message_and_exit("serialize() failed");
  }

  List l3 = { NULL };

  printf("Loading the input list from the serialized file...\n");
  if (not deserialize(&l3, serialized_filename)) {
    print_error_message_and_exit("deserialize() failed");
  }

  printf("List contents: ");
  foreach(l3, print_with_space_as_separator);
  printf("\n");

  assert(list_equal(l2, l3));

  list_free(list);
  list_free(l2);
  list_free(l3);
}

#pragma clang diagnostic pop