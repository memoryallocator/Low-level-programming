#ifndef ASSIGNMENT_IMAGE_ROTATION_VIEW_HEADER_IMAGE_H_
#define ASSIGNMENT_IMAGE_ROTATION_VIEW_HEADER_IMAGE_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

struct BgrImage {
	uint64_t height;
	uint64_t width;
	struct BgrPixel *pixels;
};

struct BgrPixel {
	uint8_t b;
	uint8_t g;
	uint8_t r;
};

#endif // ASSIGNMENT_IMAGE_ROTATION_VIEW_HEADER_IMAGE_H_
