#ifndef ASSIGNMENT_IMAGE_ROTATION__Bgr_IMAGE_TRANSFORMATIONS_H_
#define ASSIGNMENT_IMAGE_ROTATION__Bgr_IMAGE_TRANSFORMATIONS_H_

#include <stdbool.h>
#include <assert.h>
#include <iso646.h>
#include <math.h>
#include <memory.h>

#include "bgr_image.h"
#include "util.h"
#include "matrix.h"

struct BgrImageTransformationPoint {
	int64_t x;
	int64_t y;
};

enum BgrImageTransformationStatus {
	kBgrImageTransformationOk = 0,
	kBgrImageTransformationMemoryError,
	kBgrImageTransformationNullAsInput,
	kBgrImageTransformationUnderlyingError,
	kBgrImageTransformationBadInput
};

enum BgrImageTransformationStatus BgrImageTransformationRotate(const struct BgrImage *image,
																															 double degrees,
																															 struct BgrPixel filler_pixel,
																															 bool crop,
																															 struct BgrImage *output);

enum BgrImageTransformationStatus BgrImageTransformationCrop(const struct BgrImage *image,
																														 struct BgrImageTransformationPoint top_left_corner,
																														 struct BgrImageTransformationPoint bottom_right_corner,
																														 struct BgrImage *output);

#endif // ASSIGNMENT_IMAGE_ROTATION__Bgr_IMAGE_TRANSFORMATIONS_H_
