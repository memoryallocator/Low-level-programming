#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>

void print_error_message_and_exit(const char* const err_msg) {
  fprintf(stderr, "Error: %s\n", err_msg);
  exit(1);
}
