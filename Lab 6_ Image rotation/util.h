#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

uint32_t Abs(int32_t x);

uint64_t Max(uint64_t a, uint64_t b);

uint64_t Min(uint64_t a, uint64_t b);

double DegreesToRadians(double degrees);

void **Allocate2dArray(size_t height, size_t width, size_t size_of_entry);

_Noreturn void err(const char *msg, ...);

#endif
