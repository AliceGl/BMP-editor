#include "stego.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

void load_key(struct stego_key *key, char *filename, int max_size) {
  FILE *file = fopen(filename, "r");
  assert(file);
  key->size = 0;
  for (int i = 0; i < max_size; ++i) {
    if (fscanf(file, "%d %d %c", &key->keys[i].x, &key->keys[i].y, &key->keys[i].color) != 3)
      break;
    ++key->size;
  }
  fclose(file);
}

void load_msg(char *msg, char *filename, int max_size) {
  FILE *file = fopen(filename, "r");
  int size = 0;
  for (int i = 0; i < max_size; ++i) {
    if (fscanf(file, "%c", &msg[i]) != 1)
      break;
    ++size;
  }
  msg[size] = '\0';
  fclose(file);
}

int char_to_int(char c) {
  if (c == ' ')
    return 26;
  if (c == '.')
    return 27;
  if (c == ',')
    return 28;
  int x = c - 'A';
  assert(0 <= x && x < 26);
  return x;
}

void insert(struct bmp_image *image, struct stego_key *key, char *msg) {
  struct stego_pixel *cur = key->keys;
  while (*msg != '\0') {
    int code = char_to_int(*msg);
    for (int i = 0; i < 5; ++i) {
      assert(0 <= cur->x && cur->x < image->header.biWidth);
      assert(0 <= cur->y && cur->y < image->header.biHeight);
      assert(cur->color == 'R' || cur->color == 'G' || cur->color == 'B');
      bool bit = code & (1 << i);
      struct pixel *pixel = &image->table[cur->y][cur->x];
      if (cur->color == 'R' && (bool)(pixel->r & 1) != bit)
        pixel->r ^= 1;
      if (cur->color == 'G' && (bool)(pixel->g & 1) != bit)
        pixel->g ^= 1;
      if (cur->color == 'B' && (bool)(pixel->b & 1) != bit)
        pixel->b ^= 1;
      ++cur;
    }
    ++msg;
  }
}

char int_to_char(int x) {
  assert(0 <= x && x < 29);
  if (x == 26)
    return ' ';
  if (x == 27)
    return '.';
  if (x == 28)
    return ',';
  return 'A' + x;
}

void extract(struct bmp_image *image, struct stego_key *key, char *msg) {
  struct stego_pixel *cur = key->keys;
  for (int c = 0; c < key->size / 5; ++c) {
    int code = 0;
    for (int i = 0; i < 5; ++i) {
      assert(0 <= cur->x && cur->x < image->header.biWidth);
      assert(0 <= cur->y && cur->y < image->header.biHeight);
      assert(cur->color == 'R' || cur->color == 'G' || cur->color == 'B');
      struct pixel *pixel = &image->table[cur->y][cur->x];
      if (cur->color == 'R' && (pixel->r & 1))
        code |= (1 << i);
      if (cur->color == 'G' && (pixel->g & 1))
        code |= (1 << i);
      if (cur->color == 'B' && (pixel->b & 1))
        code |= (1 << i);
      ++cur;
    }
    *msg = int_to_char(code);
    ++msg;
  }
  *msg = '\0';
}

void save_msg(char *msg, char *filename) {
  FILE *file = fopen(filename, "w");
  assert(file);
  while (*msg != '\0') {
    fprintf(file, "%c", *msg);
    ++msg;
  }
  fprintf(file, "\n");
  fclose(file);
}

