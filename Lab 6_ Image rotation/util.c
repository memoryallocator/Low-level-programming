#include "util.h"

_Noreturn void err(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

void **Allocate2dArray(const size_t height, const size_t width, const size_t size_of_entry) {
	void **res = malloc(height * sizeof(void *));
	if (res == NULL) {
		return NULL;
	}

	bool every_line_was_allocated = true;
	size_t failed_to_allocate_line_number;
	for (size_t i = 0; i < height; ++i) {
		res[i] = malloc(width * size_of_entry);  // <- warning: can cause an overflow
		if (res[i] == NULL) {
			failed_to_allocate_line_number = i;
			every_line_was_allocated = false;
			break;
		}
	}

	if (every_line_was_allocated) {
		return res;
	}

	for (size_t i = 0; i < failed_to_allocate_line_number; ++i) {
		free(res[i]);
	}
	return NULL;
}

double DegreesToRadians(const double degrees) {
	return degrees * 3.14159265358979323846 / 180;
}

uint32_t Abs(const int32_t x) {
	if (x == INT32_MIN) {
		return 1 + (uint32_t)INT32_MAX;
	}

	return (uint32_t)abs(x);
}

uint64_t Max(const uint64_t a, const uint64_t b) {
	if (a > b) {
		return a;
	}
	return b;
}

uint64_t Min(const uint64_t a, const uint64_t b) {
	if (a == Max(a, b)) {
		return b;
	}
	return a;
}
