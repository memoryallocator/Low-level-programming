#ifndef _BMP_H_
#define _BMP_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "util.h"
#include "bgr_image.h"

enum BmpProcessingStatus {
	kBmpProcessingOk = 0,
	kBmpProcessingInvalidHeader,
	kBmpProcessingNullAsInput,
	kBmpProcessingUnexpectedEofReadingHeader,
	kBmpProcessingUnexpectedEofReadingImage,
	kBmpProcessingMemoryError,
	kBmpProcessingErrorWritingToFile
};

void BmpExplainError(enum BmpProcessingStatus, FILE *);

#define PRI_SPECIFIER(e) (_Generic( (e), uint16_t: "%" PRIu16, uint32_t: "%" PRIu32, \
    int32_t: "%" PRId32,                                                             \
    char[2]: "%.2s", \
    default: "NOT IMPLEMENTED" ))

#define PRINT_FIELD(t, name) \
    fprintf( f, "%-17s: ",  # name ); \
    fprintf( f, PRI_SPECIFIER( header-> name ) , header-> name );\
    fprintf( f, "\n");

#define FOR_BMP_HEADER(FOR_FIELD) \
        FOR_FIELD( uint32_t,file_size)\
        FOR_FIELD( uint32_t,reserved)\
        FOR_FIELD( uint32_t,off_bits)\
        FOR_FIELD( uint32_t,header_size)\
        FOR_FIELD( int32_t,image_width)\
        FOR_FIELD( int32_t,image_height)\
        FOR_FIELD( uint16_t,image_planes)\
        FOR_FIELD( uint16_t,image_bit_count)\
        FOR_FIELD( uint32_t,image_compression)\
        FOR_FIELD( uint32_t,image_size)\
        FOR_FIELD( uint32_t,x_pixels_per_meter)\
        FOR_FIELD( uint32_t,y_pixels_per_meter)\
        FOR_FIELD( uint32_t,colors_used)\
        FOR_FIELD( uint32_t,colors_important)

#define DECLARE_FIELD(t, n) t n ;

struct __attribute__((packed)) BmpHeader {
	char file_type[2];
	FOR_BMP_HEADER(DECLARE_FIELD)
};

struct Bmp {
	struct BmpHeader header;
	uint8_t **bitmap_image;
};

enum BmpProcessingStatus BmpGetHeaderFromOpenedFile(FILE *file, struct BmpHeader *header);

enum BmpProcessingStatus BmpVerifyHeader(const struct BmpHeader *header);

enum BmpProcessingStatus BmpFromHeader(const struct BmpHeader *header,
																			 bool allocate_bitmap_image,
																			 struct Bmp *bmp);

enum BmpProcessingStatus BmpGetBitmapImageFromOpenedFile(FILE *file, struct Bmp *bmp);

enum BmpProcessingStatus BmpGetFromOpenedFile(FILE *file, struct Bmp *bmp);

enum BmpProcessingStatus BmpWriteToOpenedFile(FILE *file, const struct Bmp *bmp);

bool BmpEquals(const struct Bmp *lhs, const struct Bmp *rhs);

bool BmpHeaderEquals(const struct BmpHeader *lhs, const struct BmpHeader *rhs);

void BmpPrint(const struct Bmp *bmp, FILE *f);

uint64_t BmpCalculatePaddedWidthInBytes(const struct BmpHeader *header);

enum BmpProcessingStatus BmpToImage(const struct Bmp *bmp, struct BgrImage *image);

enum BmpProcessingStatus BmpFromImage(const struct BgrImage *image, struct Bmp *output);

#endif
