#ifndef BMP_H_
#define BMP_H_

#include <stdint.h>

#pragma pack(push, 1)

struct bf_header { 
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
};

struct bi_header {
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
};

struct pixel {
  int8_t b;
  int8_t g;
  int8_t r;
};

#pragma pack(pop)

struct bmp_image {
  struct bf_header file_header;
  struct bi_header header;
  struct pixel **table;
};

void load_bmp(struct bmp_image *image,  char *filename);

void crop(struct bmp_image *image, int x, int y, int h, int w);

void rotate(struct bmp_image *image);

void save_bmp(struct bmp_image *image, char *filename);

void free_table(struct bmp_image *image);

#endif
