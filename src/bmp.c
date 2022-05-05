#include "bmp.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

const int PAD_BYTES = 4;

int calc_padding(int w) {
  return (PAD_BYTES - w * sizeof(struct pixel) % PAD_BYTES) % PAD_BYTES;
}

void change_headers(struct bmp_image *image, int w, int h) {
  image->header.biHeight = h;
  image->header.biWidth = w;
  image->header.biSizeImage = (sizeof(struct pixel) * w + calc_padding(w)) * h;
  image->file_header.bfSize = sizeof(struct bf_header) + sizeof(struct bi_header) + image->header.biSizeImage;
}

void load_bmp(struct bmp_image *image, char *filename) {
  FILE *file = fopen(filename, "rb");
  assert(file);
  assert(fread(&image->file_header, sizeof(image->file_header), 1, file) == 1);
  assert(fread(&image->header, sizeof(image->header), 1, file) == 1);
  assert(fseek(file, image->file_header.bfOffBits, SEEK_SET) == 0);
  image->table = malloc(sizeof(struct pixel *) * image->header.biHeight);
  assert(image->table);
  int padding_size = calc_padding(image->header.biWidth);
  for (int y = image->header.biHeight - 1; y >= 0; --y) {
    image->table[y] = malloc(sizeof(struct pixel) * image->header.biWidth);
    assert(image->table[y]);
    assert(fread(image->table[y], sizeof(struct pixel) * image->header.biWidth, 1, file) == 1);
    if (padding_size != 0)
      assert(fseek(file, padding_size, SEEK_CUR) == 0);
  }
  fclose(file);
}

void free_table(struct bmp_image *image) {
  for (int y = 0; y < image->header.biHeight; ++y)
    free(image->table[y]);
  free(image->table);
}

void crop(struct bmp_image *image, int x, int y, int w, int h) {
  assert(0 <= x && x < x + w && x + w <= image->header.biWidth);
  assert(0 <= y && y < y + h && y + h <= image->header.biHeight);
  struct pixel **cropped = malloc(sizeof(struct pixel *) * h);
  assert(cropped);
  for (int y0 = 0; y0 < h; ++y0) {
    cropped[y0] = malloc(sizeof(struct pixel) * w);
    assert(cropped[y0]);
    memcpy(cropped[y0], &image->table[y + y0][x], sizeof(struct pixel) * w);
  }
  free_table(image);
  image->table = cropped;
  change_headers(image, w, h);
}

void rotate(struct bmp_image *image) {
  int h = image->header.biWidth, w = image->header.biHeight;
  struct pixel **rotated = malloc(sizeof(struct pixel *) * h);
  assert(rotated);
  for (int y = 0; y < h; ++y) {
    rotated[y] = malloc(sizeof(struct pixel) * w);
    assert(rotated[y]);
    for (int x = 0; x < w; ++x)
      rotated[y][x] = image->table[w - x - 1][y];
  }
  free_table(image);
  image->table = rotated;
  change_headers(image, w, h);
}

void save_bmp(struct bmp_image *image, char *filename) {
  FILE *file = fopen(filename, "wb");
  assert(file);
  fwrite(&image->file_header, sizeof(image->file_header), 1, file);
  fwrite(&image->header, sizeof(image->header), 1, file);
  int padding_size = calc_padding(image->header.biWidth);
  char *paddings = calloc(1, padding_size);
  for (int y = image->header.biHeight - 1; y >= 0; --y) {
    fwrite(image->table[y], sizeof(struct pixel) * image->header.biWidth, 1, file);
    if (padding_size != 0)
      fwrite(paddings, padding_size, 1, file);
  }
  free(paddings);
  free_table(image);
  fclose(file);
}

