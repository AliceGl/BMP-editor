#ifndef STEGO_H_
#define STEGO_H_

#include "bmp.h"

struct stego_pixel {
  int x;
  int y;
  char color;
};

struct stego_key {
  int size;
  struct stego_pixel *keys;
};

void load_key(struct stego_key *key, char *filename, int max_size);

void load_msg(char *msg, char *filename, int max_size);

void insert(struct bmp_image *image, struct stego_key *key, char *msg);

void extract(struct bmp_image *image, struct stego_key *key, char *msg);

void save_msg(char *msg, char *filename);

#endif
