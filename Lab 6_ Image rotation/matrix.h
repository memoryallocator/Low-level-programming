#ifndef ASSIGNMENT_IMAGE_ROTATION__MATRIX_H_
#define ASSIGNMENT_IMAGE_ROTATION__MATRIX_H_

#include <stddef.h>
#include <assert.h>
#include <iso646.h>
#include <math.h>

#include "util.h"

enum MatrixOperationStatus {
	kMatrixOperationOk = 0,
	kMatrixOperationMemoryError,
	kMatrixOperationNullAsInput,
	kMatrixOperationDimensionMismatch
};

struct Matrix {
	size_t rows;
	size_t columns;
	double **data;
};

struct Matrix *MatrixInverseInplace(struct Matrix *matrix);

struct Matrix *MatrixMultiplyByScalar(struct Matrix *matrix, double a);

enum MatrixOperationStatus MatrixGet2dRotation(double radians, struct Matrix *output);

enum MatrixOperationStatus MatrixMultiply(const struct Matrix *A, const struct Matrix *B, struct Matrix *output);

enum MatrixOperationStatus MatrixSetShape(struct Matrix *matrix, size_t rows, size_t columns);

#endif // ASSIGNMENT_IMAGE_ROTATION__MATRIX_H_
