#ifndef LOW_LEVEL_UTILITIES_H
#define LOW_LEVEL_UTILITIES_H

#define IS_INT64_T(x) _Generic((x), int64_t: true, default: false)

void print_error_message_and_exit(const char* err_msg);

#endif  // LOW_LEVEL_UTILITIES_H