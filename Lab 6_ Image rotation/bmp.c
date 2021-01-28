#include "bmp.h"

void BmpHeaderPrint(const struct BmpHeader *const header, FILE *f) {
	fprintf(f, "%.2s\n", header->file_type);
	FOR_BMP_HEADER(PRINT_FIELD)
#undef PRI_SPECIFIER
#undef PRINT_FIELD
#undef FOR_BMP_HEADER
#undef DECLARE_FIELD
}

static bool BmpReadHeader(FILE *f, struct BmpHeader *const header) {
	if (fread(header, sizeof(struct BmpHeader), 1, f) == 1) {
		return true;
	}
	return false;
}

enum BmpProcessingStatus BmpGetHeaderFromOpenedFile(FILE *const file, struct BmpHeader *const header) {
	if (file == NULL) {
		return kBmpProcessingNullAsInput;
	}

	if (BmpReadHeader(file, header)) {
		return kBmpProcessingOk;
	}
	return kBmpProcessingUnexpectedEofReadingHeader;
}

enum BmpProcessingStatus BmpVerifyHeader(const struct BmpHeader *const header) {
	if (header == NULL) {
		return kBmpProcessingNullAsInput;
	}

	{
		if (header->file_type[0] != 'B' {
			or
		}
		header->file_type[1] != 'M') {
			return kBmpProcessingInvalidHeader;
		}
	}

	if (header->file_size < sizeof(struct BmpHeader)) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->header_size > sizeof(struct BmpHeader)) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->image_planes != 1) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->image_bit_count != 24) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->image_compression != 0) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->colors_used != 0) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->colors_important != 0) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->x_pixels_per_meter > 3780 {
		or
	}
	header->y_pixels_per_meter > 3780) {
		return kBmpProcessingInvalidHeader;
	}

	if (header->image_width < 0) {
		return kBmpProcessingInvalidHeader;
	}
	return kBmpProcessingOk;
}

static uint8_t BmpGetPixelSizeInBytes(const struct BmpHeader *const header) {
	return (uint8_t)(header->image_bit_count / 8);
}

static uint8_t BmpCalculatePadding(const struct BmpHeader *const header) {
	const uint8_t kBytesPerPixel = BmpGetPixelSizeInBytes(header);
	const uint64_t kPixelsWidthInBytes = Abs(header->image_width) * kBytesPerPixel;

	const uint8_t kExcess = kPixelsWidthInBytes % 4;
	if (kExcess == 0) {
		return 0;
	}
	return (uint8_t)(4 - kExcess);
}

uint64_t BmpCalculatePaddedWidthInBytes(const struct BmpHeader *header) {
	return BmpGetPixelSizeInBytes(header) * Abs(header->image_width)
			+ BmpCalculatePadding(header);
}

static uint8_t **BmpAllocateBitmapImage(const struct BmpHeader *const header) {
	const uint32_t kHeight = Abs(header->image_height);
	const uint64_t kWidthInBytes = BmpCalculatePaddedWidthInBytes(header);
	return (uint8_t **)Allocate2dArray(kHeight, kWidthInBytes, sizeof(uint8_t));
}

enum BmpProcessingStatus BmpFromHeader(const struct BmpHeader *header,
																			 const bool allocate_bitmap_image,
																			 struct Bmp *const bmp) {
	if (bmp == NULL || header == NULL) {
		return kBmpProcessingNullAsInput;
	}

	bmp->header = *header;

	if (allocate_bitmap_image) {
		bmp->bitmap_image = BmpAllocateBitmapImage(header);
		if (bmp->bitmap_image == NULL) {
			return kBmpProcessingMemoryError;
		}
	}
	return kBmpProcessingOk;
}

static bool BmpReadBitmapImage(FILE *f,
															 const uint32_t height,
															 const uint64_t width,
															 uint8_t **const bitmap_image) {
	for (uint32_t i = 0; i < height; ++i) {
		if (fread(bitmap_image[i], sizeof(uint8_t), width, f) != width) {
			return false;
		}
	}
	return true;
}

enum BmpProcessingStatus BmpGetBitmapImageFromOpenedFile(FILE *const file, struct Bmp *const bmp) {
	if (bmp == NULL {
		or
	}
	file == NULL) {
		return kBmpProcessingNullAsInput;
	}

	const uint64_t kWidthInBytes = BmpCalculatePaddedWidthInBytes(&(bmp->header));

	if (bmp->bitmap_image == NULL) {
		bmp->bitmap_image = BmpAllocateBitmapImage(&(bmp->header));
		if (bmp->bitmap_image == NULL) {
			return kBmpProcessingMemoryError;
		}
	}

	if (not BmpReadBitmapImage(file, Abs(bmp->header.image_height), kWidthInBytes, bmp->bitmap_image)) {
		return kBmpProcessingUnexpectedEofReadingImage;
	}
	return kBmpProcessingOk;
}

enum BmpProcessingStatus BmpGetFromOpenedFile(FILE *file, struct Bmp *const bmp) {
	{
		const enum BmpProcessingStatus kBmpGetHeaderResult = BmpGetHeaderFromOpenedFile(file, &(bmp->header));
		if (kBmpGetHeaderResult != kBmpProcessingOk) {
			return kBmpGetHeaderResult;
		}
	}

	{
		const enum BmpProcessingStatus kBmpVerificationResult = BmpVerifyHeader(&(bmp->header));
		if (kBmpVerificationResult != kBmpProcessingOk) {
			return kBmpVerificationResult;
		}
	}

	{
		const enum BmpProcessingStatus kBmpFromHeaderConversionResult = BmpFromHeader(&(bmp->header), true, bmp);
		if (kBmpFromHeaderConversionResult != kBmpProcessingOk) {
			return kBmpFromHeaderConversionResult;
		}
	}

	{
		const enum BmpProcessingStatus kBmpGetPixelsFromFileResult = BmpGetBitmapImageFromOpenedFile(file, bmp);
		if (kBmpGetPixelsFromFileResult != kBmpProcessingOk) {
			return kBmpGetPixelsFromFileResult;
		}
	}
	return kBmpProcessingOk;
}

enum BmpProcessingStatus BmpToImage(const struct Bmp *const bmp, struct BgrImage *const image) {
	if (bmp == NULL {
		or
	}
	image == NULL) {
		return kBmpProcessingNullAsInput;
	}

	const uint32_t kAbsImageHeight = Abs(bmp->header.image_height);
	image->height = kAbsImageHeight;
	image->width = (uint32_t)bmp->header.image_width;

	free(image->pixels);
	image->pixels = calloc(image->width, kAbsImageHeight * sizeof(struct BgrPixel));  // avoiding overflow
	if (image->pixels == NULL) {
		return kBmpProcessingMemoryError;
	}

	const bool kTopToBottom = (bmp->header.image_height < 0) ? true : false;

	for (uint32_t i = 0; i < kAbsImageHeight; ++i) {
		const uint32_t kCurrOutputLine = kTopToBottom ? i : (kAbsImageHeight - i - 1);
		memcpy(&(image->pixels[kCurrOutputLine * image->width]),
					 bmp->bitmap_image[i],
					 image->width * sizeof(struct BgrPixel));
	}
	return kBmpProcessingOk;
}

enum BmpProcessingStatus BmpWriteToOpenedFile(FILE *file, const struct Bmp *const bmp) {
	if (file == NULL {
		or
	}
	bmp == NULL) {
		return kBmpProcessingNullAsInput;
	}

	if (fwrite(&(bmp->header), sizeof(bmp->header), 1, file) != 1) {
		return kBmpProcessingErrorWritingToFile;
	}

	const uint64_t kPaddedWidthInBytes = BmpCalculatePaddedWidthInBytes(&(bmp->header));

	for (uint32_t i = 0; i < Abs(bmp->header.image_height); ++i) {
		if (fwrite(bmp->bitmap_image[i], kPaddedWidthInBytes, 1, file) != 1) {
			return kBmpProcessingErrorWritingToFile;
		}
	}
	return kBmpProcessingOk;
}

bool BmpHeaderEquals(const struct BmpHeader *const lhs, const struct BmpHeader *const rhs) {
#define FIELD_EQ(field) lhs->field == rhs->field
	return FIELD_EQ(file_size)
	and
	FIELD_EQ(reserved)
	and
	FIELD_EQ(off_bits)
	and
	FIELD_EQ(header_size)
	and
	FIELD_EQ(image_width)
	and
	FIELD_EQ(image_height)
	and
	FIELD_EQ(image_planes)
	and
	FIELD_EQ(image_bit_count)
	and
	FIELD_EQ(image_compression)
	and
	FIELD_EQ(image_size)
	and
	FIELD_EQ(x_pixels_per_meter)
	and
	FIELD_EQ(y_pixels_per_meter)
	and
	FIELD_EQ(colors_used)
	and
	FIELD_EQ(colors_important)
	and
	FIELD_EQ(file_type[0])
	and
	FIELD_EQ(file_type[1]);
#undef FIELD_EQ
}

bool BmpEquals(const struct Bmp *const lhs, const struct Bmp *const rhs) {
	if (lhs == rhs) {
		return true;
	}

	if (not BmpHeaderEquals( {
		&(lhs->header), &(rhs->header)
	})) {
		return false;
	}

	const uint64_t kWidthInBytesPaddingExcluded = (uint64_t)lhs->header.image_width
			* BmpGetPixelSizeInBytes(&(lhs->header));

	for (uint32_t i = 0; i < Abs(lhs->header.image_height); ++i) {
		if (memcmp(lhs->bitmap_image[i], rhs->bitmap_image[i], kWidthInBytesPaddingExcluded) != 0) {
			return false;
		}
	}
	return true;
}

void BmpPrint(const struct Bmp *const bmp, FILE *f) {
	BmpHeaderPrint(&(bmp->header), f);

	if (bmp->header.image_width == 0) {
		return;
	}

	const uint64_t kPaddedWidthInBytes = BmpCalculatePaddedWidthInBytes(&(bmp->header));

	for (uint32_t i = 0; i < Abs(bmp->header.image_height); ++i) {
		fprintf(f, "%02X", bmp->bitmap_image[i][0]);
		for (uint64_t j = 1; j < kPaddedWidthInBytes; ++j) {
			fprintf(f, " %02X", bmp->bitmap_image[i][j]);
		}
		fprintf(f, "\n");
	}
}

void BmpExplainError(const enum BmpProcessingStatus error, FILE *const f) {
	if (error == kBmpProcessingOk) {
		fprintf(f, "Bmp processing is OK\n");
		return;
	}

	fprintf(f, "Something went wrong...\n");

	switch (error) {
		case kBmpProcessingUnexpectedEofReadingHeader:
			fprintf(f, "Header too short\n");
			break;

		case kBmpProcessingUnexpectedEofReadingImage:
			fprintf(f, "File too short\n");
			break;

		case kBmpProcessingInvalidHeader:
			fprintf(f, "Something is wrong with the BMP file header\n");
			break;

		case kBmpProcessingMemoryError:
			fprintf(f, "Not enough memory\n");
			break;

		case kBmpProcessingErrorWritingToFile:
			fprintf(f, "Failed to write to file\n");
			break;

		case kBmpProcessingNullAsInput:
			fprintf(f, "Internal error\n");
			break;

		default:
			break;
	}
}

enum BmpProcessingStatus BmpFromImage(const struct BgrImage *const image, struct Bmp *const output) {
	if (image == NULL {
		or
	}
	output == NULL) {
		return kBmpProcessingNullAsInput;
	}

	struct BmpHeader output_header;
	output_header.file_type[0] = 'B';
	output_header.file_type[1] = 'M';

	output_header.reserved = 0;
	output_header.off_bits = 54;
	output_header.header_size = 40;
	output_header.image_bit_count = 24;
	output_header.image_width = (int32_t)image->width;
	output_header.image_height = -(int32_t)image->height;
	output_header.image_planes = 1;
	output_header.image_compression = 0;

	output_header.x_pixels_per_meter = 2835;
	output_header.y_pixels_per_meter = 2835;
	output_header.colors_important = 0;
	output_header.colors_used = 0;

	free(output->bitmap_image);

	const uint64_t kPaddedWidthInBytes = BmpCalculatePaddedWidthInBytes(&output_header);
	output_header.image_size = (uint32_t)image->height * (uint32_t)kPaddedWidthInBytes;
	output_header.file_size = output_header.image_size + output_header.off_bits;

	output->bitmap_image = (uint8_t **)Allocate2dArray(image->height,
																										 kPaddedWidthInBytes,
																										 sizeof(uint8_t));
	if (output->bitmap_image == NULL) {
		return kBmpProcessingMemoryError;
	}

	output->header = output_header;
	for (int32_t i = 0; (uint32_t)i < image->height; ++i) {
		memcpy(output->bitmap_image[i],
					 &(image->pixels[(uint32_t)i * image->width]),
					 image->width * sizeof(struct BgrPixel));
	}
	return kBmpProcessingOk;
}
