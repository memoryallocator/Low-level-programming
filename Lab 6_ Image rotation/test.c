#include "test.h"

void RunTest() {
	const struct BmpHeader kTestHeader = {
			"BM",
			70,
			0,
			54,
			40,
			2,
			2,
			1,
			24,
			0,
			16,
			2835,
			2835,
			0,
			0
	};

	const uint8_t kTestBitmapImage[][8] = {
			{ 0x00, 0x00, 0xFF,
				0xFF, 0xFF, 0xFF,
				0x00, 0x00 },

			{ 0xFF, 0x00, 0x00,
				0x00, 0xFF, 0x00,
				0x00, 0x00 }
	};

	struct Bmp test_bmp = {
			kTestHeader,
			NULL
	};

	const uint64_t kPaddedWidthInBytes = BmpCalculatePaddedWidthInBytes(&(test_bmp.header));
	test_bmp.bitmap_image = (uint8_t **)Allocate2dArray(Abs(test_bmp.header.image_height),
																											kPaddedWidthInBytes,
																											sizeof(uint8_t));

	const uint32_t kAbsImageHeight = Abs(test_bmp.header.image_height);

	for (uint32_t i = 0; i < kAbsImageHeight; ++i) {
		for (uint64_t j = 0; j < kPaddedWidthInBytes; ++j) {
			const uint32_t kCurrLine = i;
			const uint8_t kCurrByte = kTestBitmapImage[kCurrLine][j];
			test_bmp.bitmap_image[i][j] = kCurrByte;
		}
	}

	BmpPrint(&test_bmp, stderr);

	FILE *const f = fopen("test.bmp", "w+b");
	assert(BmpWriteToOpenedFile(f, &test_bmp) == kBmpProcessingOk);
	fseek(f, 0, SEEK_SET);

	struct Bmp read_bmp;
	const enum BmpProcessingStatus kBmpReadStatus = BmpGetFromOpenedFile(f, &read_bmp);
	fclose(f);
	assert(kBmpReadStatus == kBmpProcessingOk);

	fprintf(stderr, "\n");
	BmpPrint(&read_bmp, stderr);
	assert(BmpEquals(&test_bmp, &read_bmp));
}
