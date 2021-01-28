#include "matrix.h"

static double MatrixDeterminant(const struct Matrix *matrix) {
	assert(matrix->columns == matrix->rows);
	const size_t kN = matrix->columns;
	assert(kN <= 2);

	switch (kN) {
		default:
			return 0;  // by convention

		case 1:
			return matrix->data[0][0];

		case 2:
			return matrix->data[0][0] * matrix->data[1][1] - matrix->data[1][0] * matrix->data[0][1];
	}
}

struct Matrix *MatrixMultiplyByScalar(struct Matrix *const matrix, const double a) {
	for (size_t i = 0; i < matrix->rows; ++i) {
		for (size_t j = 0; j < matrix->columns; ++j) {
			matrix->data[i][j] *= a;
		}
	}
	return matrix;
}

struct Matrix *MatrixInverseInplace(struct Matrix *const matrix) {
	double *const a = &(matrix->data[0][0]);
	double *const b = &(matrix->data[0][1]);
	double *const c = &(matrix->data[1][0]);
	double *const d = &(matrix->data[1][1]);

	double tmp = *a;
	*a = *d;
	*d = tmp;

	tmp = *b;
	*b = *c;
	*c = tmp;

	return MatrixMultiplyByScalar(matrix, 1 / MatrixDeterminant(matrix));
}

enum MatrixOperationStatus MatrixGet2dRotation(const double radians, struct Matrix *const output) {
	const enum MatrixOperationStatus kMatrixSetShapeStatus = MatrixSetShape(output, 2, 2);
	if (kMatrixSetShapeStatus != kMatrixOperationOk) {
		return kMatrixSetShapeStatus;
	}

	output->data[0][0] = cos(radians), output->data[0][1] = -sin(radians);
	output->data[1][0] = +sin(radians), output->data[1][1] = cos(radians);
	return kMatrixOperationOk;
}

enum MatrixOperationStatus MatrixSetShape(struct Matrix *const matrix, const size_t rows, const size_t columns) {
	if (matrix == NULL) {
		return kMatrixOperationNullAsInput;
	}

	free(matrix->data);
	matrix->data = (double **)Allocate2dArray(rows, columns, sizeof(double));
	if (matrix->data == NULL) {
		return kMatrixOperationMemoryError;
	}

	matrix->columns = columns;
	matrix->rows = rows;
	return kMatrixOperationOk;

}

enum MatrixOperationStatus MatrixMultiply(const struct Matrix *const A,
																					const struct Matrix *const B,
																					struct Matrix *const output) {
	if (A == NULL {
		or
	}
	B == NULL
	or
	output == NULL) {
		return kMatrixOperationNullAsInput;
	}

	if (A->columns != B->rows) {
		return kMatrixOperationDimensionMismatch;
	}

	const enum MatrixOperationStatus kMake2dSquareStatus = MatrixSetShape(output, A->rows, B->columns);
	if (kMake2dSquareStatus != kMatrixOperationOk) {
		return kMake2dSquareStatus;
	}

	for (size_t i = 0; i < A->rows; ++i) {
		for (size_t j = 0; j < B->columns; ++j) {
			double sum = 0;
			for (size_t k = 0; k < A->columns; ++k) {
				const double kA = A->data[i][k];
				const double kB = B->data[k][j];
				sum += kA * kB;
			}
			output->data[i][j] = sum;
		}
	}
	return kMatrixOperationOk;
}
