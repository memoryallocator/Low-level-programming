#include <stdio.h>

#include "bmp.h"
#include "util.h"
#include "test.h"
#include "bgr_image_transformations.h"

void PrintUsage() {
	fprintf(stderr, "Usage: ./rotate BMP_FILE_NAME [DEGREES]\n");
}

int main(int argc, char **argv) {
	if (argc != 2) {
		PrintUsage();

		if (argc < 2) {
			err("Not enough arguments\n");
		}

		if (argc > 3) {
			err("Too many arguments\n");
		}
	}

	if (strcmp(argv[1], "--test") == 0) {
		RunTest();
		return EXIT_SUCCESS;
	}

	FILE *bmp_file = fopen(argv[1], "rb");
	if (bmp_file == NULL) {
		fprintf(stderr, "Unable to open file %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	struct Bmp bmp;
	{
		const enum BmpProcessingStatus kBmpReadingFromFileStatus = BmpGetFromOpenedFile(bmp_file, &bmp);
		fclose(bmp_file);

		if (kBmpReadingFromFileStatus != kBmpProcessingOk) {
			BmpExplainError(kBmpReadingFromFileStatus, stderr);
			return EXIT_FAILURE;
		}
	}

	struct BgrImage image = { 0 };
	{
		const enum BmpProcessingStatus kBmpConvertingToImageStatus = BmpToImage(&bmp, &image);
		if (kBmpConvertingToImageStatus != kBmpProcessingOk) {
			BmpExplainError(kBmpConvertingToImageStatus, stderr);
			return EXIT_FAILURE;
		}

		BmpFromImage(&image, &bmp);

		const char kRotatedImageFileNamePrefix[] = "rotated_";
		const size_t kRotatedImageFileNamePrefixLen = strlen(kRotatedImageFileNamePrefix);

		char *rotated_image_file_name = calloc(kRotatedImageFileNamePrefixLen + strlen(argv[1]) + 1, sizeof(char));
		if (rotated_image_file_name == NULL) {
			fprintf(stderr, "Not enough memory\n");
			return EXIT_FAILURE;
		}
	}

	struct BgrImage rotated = { 0 };
	const struct BgrPixel kBgrImageTransformationFillerPixel = {
			0xFF, 0xFF, 0xFF
	};
	{
		const double kDegrees = (argc == 3) ? strtod(argv[2], NULL) : 90;
		const enum BgrImageTransformationStatus kRotationStatus = BgrImageTransformationRotate(&image,
																																													 kDegrees,
																																													 kBgrImageTransformationFillerPixel,
																																													 true,
																																													 &rotated);
		if (kRotationStatus != kBgrImageTransformationOk) {
			fprintf(stderr, "Something went wrong\n");
			return EXIT_FAILURE;
		}
	}

	{
		const enum BmpProcessingStatus kBmpConvertingFromRotatedImageStatus = BmpFromImage(&rotated, &bmp);
		if (kBmpConvertingFromRotatedImageStatus != kBmpProcessingOk) {
			BmpExplainError(kBmpConvertingFromRotatedImageStatus, stderr);
			return EXIT_FAILURE;
		}
	}

	const char kRotatedImageFileNamePrefix[] = "rotated_";
	const size_t kRotatedImageFileNamePrefixLen = strlen(kRotatedImageFileNamePrefix);

	char *rotated_image_file_name = calloc(kRotatedImageFileNamePrefixLen + strlen(argv[1]) + 1, sizeof(char));
	if (rotated_image_file_name == NULL) {
		fprintf(stderr, "Not enough memory\n");
		return EXIT_FAILURE;
	}

	strcpy(rotated_image_file_name, kRotatedImageFileNamePrefix);
	strcat(rotated_image_file_name, argv[1]);

	FILE *rotated_image_file = fopen(rotated_image_file_name, "wb");
	if (rotated_image_file == NULL) {
		fprintf(stderr, "Failed to create/overwrite the output file\n");
		return EXIT_FAILURE;
	}

	BmpWriteToOpenedFile(rotated_image_file, &bmp);
	fclose(rotated_image_file);
}
