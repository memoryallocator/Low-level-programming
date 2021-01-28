#include "bgr_image_transformations.h"

#define PROCESS_MATRIX_ERROR(status) { \
  if ((status) != kMatrixOperationOk) { \
    if ((status) == kMatrixOperationMemoryError) {            \
      return kBgrImageTransformationMemoryError; \
    }                                                       \
    return kBgrImageTransformationUnderlyingError;           \
  }         \
}

#define PROCESS_TRANSFORMATION_ERROR(status) { \
  if ((status) != kBgrImageTransformationOk) {             \
    return status;                \
  } \
}

static struct BgrImageTransformationPoint BgrImageTransformationGetRelativePos(
		const struct BgrImage *const image,
		struct BgrImageTransformationPoint p) {
	const struct BgrImageTransformationPoint kRes = { p.x - (int64_t)image->width / 2,
																										(int64_t)image->height / 2 - p.y };
	return kRes;
}

static struct BgrImageTransformationPoint BgrImageTransformationGetAbsolutePos(const struct BgrImage *const image,
																																							 const struct BgrImageTransformationPoint pos) {
	const int64_t kAbsPosX = pos.x + (int64_t)(image->width / 2);
	const int64_t kAbsPosY = (int64_t)(image->height / 2) - pos.y;

	const struct BgrImageTransformationPoint kRes = { kAbsPosX, kAbsPosY };
	return kRes;
}

static uint64_t BgrImageTransformationCalculateRotationOutputSideLength(const struct BgrImage *const image) {
	const uint64_t kLongestSideOfInput = Max(image->width, image->height);
	const double kLengthOfDiagonalAssumingInputIsSquare = (double)kLongestSideOfInput * sqrt(2);

	const uint64_t kSideOfOutput = (uint64_t)ceil(kLengthOfDiagonalAssumingInputIsSquare);
	return kSideOfOutput;
}

static enum BgrImageTransformationStatus BgrImageTransformationInitializeSquareImage(const uint64_t side,
																																										 struct BgrImage *const image) {
	image->height = side;
	image->width = side;

	free(image->pixels);
	image->pixels = calloc(side, side * sizeof(struct BgrPixel));
	if (image->pixels == NULL) {
		return kBgrImageTransformationMemoryError;
	}
	return kBgrImageTransformationOk;
}

enum BgrImageTransformationStatus BgrImageTransformationCrop(const struct BgrImage *const image,
																														 const struct BgrImageTransformationPoint top_left_corner,
																														 const struct BgrImageTransformationPoint bottom_right_corner,
																														 struct BgrImage *const output) {
	if (image == NULL {
		or
	}
	output == NULL) {
		return kBgrImageTransformationNullAsInput;
	}
	if (image == output) {
		return kBgrImageTransformationBadInput;
	}
	{
		const uint64_t kOutputHeight = (uint64_t)(bottom_right_corner.y - top_left_corner.y) + 1;
		const uint64_t kOutputWidth = (uint64_t)(bottom_right_corner.x - top_left_corner.x) + 1;
		output->height = kOutputHeight;
		output->width = kOutputWidth;

		free(output->pixels);
		output->pixels = calloc(Max(kOutputHeight, kOutputWidth),
														Min(kOutputHeight, kOutputWidth) * sizeof(struct BgrPixel));  // avoiding overflow
		if (output->pixels == NULL) {
			return kBgrImageTransformationMemoryError;
		}
	}

	for (uint64_t i = 0; i < output->height; ++i) {
		const size_t kCopyFromLineNumber = (size_t)top_left_corner.y + i;
		const size_t kCopyFromIdx = kCopyFromLineNumber * image->width + (size_t)top_left_corner.x;
		memcpy(&(output->pixels[i * output->width]),
					 &(image->pixels[kCopyFromIdx]),
					 output->width * sizeof(struct BgrPixel));
	}
	return kBgrImageTransformationOk;
}

static enum MatrixOperationStatus BgrImageTransformationGetInverseRotationMatrix(const double radians,
																																								 struct Matrix *const output) {
	const enum MatrixOperationStatus kStatus = MatrixGet2dRotation(radians, output);
	if (kStatus != kMatrixOperationOk) {
		return kStatus;
	}

	MatrixInverseInplace(output);
	return kMatrixOperationOk;
}

static enum BgrImageTransformationStatus BgrImageTransformationPointTo2dVector(const struct BgrImageTransformationPoint p,
																																							 struct Matrix *const output) {
	const enum MatrixOperationStatus kStatus = MatrixSetShape(output, 2, 1);
	PROCESS_MATRIX_ERROR(kStatus)
	output->data[0][0] = (double)p.x;
	output->data[1][0] = (double)p.y;
	return kBgrImageTransformationOk;
}

static struct BgrImageTransformationPoint BgrImageTransformation2dVectorToPoint(const struct Matrix *const v) {
	const struct BgrImageTransformationPoint kRes = { (int64_t)round(v->data[0][0]), (int64_t)round(v->data[1][0]) };
	return kRes;
}

enum BgrImageTransformationStatus BgrImageTransformationRotate(const struct BgrImage *const image,
																															 const double degrees,
																															 const struct BgrPixel filler_pixel,
																															 const bool crop,
																															 struct BgrImage *const output) {
	if (image == NULL {
		or
	}
	output == NULL) {
		return kBgrImageTransformationNullAsInput;
	}
	if (image == output) {
		return kBgrImageTransformationBadInput;
	}
	{
		const uint64_t kSideOfOutput = BgrImageTransformationCalculateRotationOutputSideLength(image);
		const enum BgrImageTransformationStatus kStatus = BgrImageTransformationInitializeSquareImage(kSideOfOutput,
																																																	output);
		PROCESS_TRANSFORMATION_ERROR(kStatus)
	}

	struct Matrix inverse_rotation_matrix = { 0 };
	{
		const enum MatrixOperationStatus kStatus = BgrImageTransformationGetInverseRotationMatrix(DegreesToRadians(degrees),
																																															&inverse_rotation_matrix);
		PROCESS_MATRIX_ERROR(kStatus)
	}

	uint64_t leftmost_position = output->width - 1;
	uint64_t rightmost_position = 0;
	uint64_t topmost_position = output->height - 1;
	uint64_t bottommost_position = 0;

	for (uint64_t i = 0; i < output->height; ++i) {
		for (uint64_t j = 0; j < output->width; ++j) {
			const struct BgrImageTransformationPoint kCurrPoint = { (int64_t)j, (int64_t)i };
			const struct BgrImageTransformationPoint
					kRelativePosOfCurrPoint = BgrImageTransformationGetRelativePos(output, kCurrPoint);

			struct Matrix curr_point_as_vector = { 0 };
			{
				const enum BgrImageTransformationStatus kStatus = BgrImageTransformationPointTo2dVector(kRelativePosOfCurrPoint,
																																																&curr_point_as_vector);
				PROCESS_TRANSFORMATION_ERROR(kStatus)
			}

			struct Matrix curr_point_as_vector_inverted = { 0 };
			{
				const enum MatrixOperationStatus kStatus = MatrixMultiply(&inverse_rotation_matrix,
																																	&curr_point_as_vector,
																																	&curr_point_as_vector_inverted);
				PROCESS_MATRIX_ERROR(kStatus)
			}

			const struct BgrImageTransformationPoint
					kCurrPointInverted = BgrImageTransformationGetAbsolutePos(image,
																																		BgrImageTransformation2dVectorToPoint(&curr_point_as_vector_inverted));

			if (kCurrPointInverted.x < 0 {
				or
			}
			kCurrPointInverted.x >= (int32_t)image->width
			or
			kCurrPointInverted.y < 0
			or
			kCurrPointInverted.y >= (int32_t)image->height) {
				output->pixels[i * output->width + j] = filler_pixel;
			} else {
				output->pixels[i * output->width + j] =
						image->pixels[(size_t)kCurrPointInverted.y * image->width + (size_t)kCurrPointInverted.x];

				leftmost_position = Min(leftmost_position, j);
				topmost_position = Min(topmost_position, i);

				rightmost_position = Max(rightmost_position, j);
				bottommost_position = Max(bottommost_position, i);
			}
		}
	}

	if (crop) {
		const struct BgrImageTransformationPoint kTopLeftCorner = { (int64_t)leftmost_position, (int64_t)topmost_position };
		const struct BgrImageTransformationPoint kBottomRightCorner = { (int64_t)rightmost_position,
																																		(int64_t)bottommost_position };
		struct BgrImage cropped_output = { 0 };
		{
			const enum BgrImageTransformationStatus kStatus = BgrImageTransformationCrop(output,
																																									 kTopLeftCorner,
																																									 kBottomRightCorner,
																																									 &cropped_output);
			PROCESS_TRANSFORMATION_ERROR(kStatus)
		}
		free(output->pixels);
		*output = cropped_output;
	}
	return kBgrImageTransformationOk;
}

#undef PROCESS_MATRIX_ERROR
#undef PROCESS_TRANSFORMATION_ERROR
