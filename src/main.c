#include "stego.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  struct bmp_image image;
  assert(strcmp(argv[1], "crop-rotate") == 0 || strcmp(argv[1], "insert") == 0 || strcmp(argv[1], "extract") == 0);
  if (strcmp(argv[1], "crop-rotate") == 0) {
    assert(argc >= 8);
    char *in_filename = argv[2], *out_filename = argv[3];
    int x = atoi(argv[4]), y = atoi(argv[5]);
    int w = atoi(argv[6]), h = atoi(argv[7]);
    load_bmp(&image, in_filename);
    crop(&image, x, y, w, h);
    rotate(&image);
    save_bmp(&image, out_filename);
    return 0;
  }
  if (strcmp(argv[1], "insert") == 0) {
    assert(argc >= 6);
    char *in_filename = argv[2], *out_filename = argv[3];
    char *key_filename = argv[4], *msg_filename = argv[5];
    load_bmp(&image, in_filename);
    int max_size = image.header.biWidth * image.header.biHeight * 3;
    struct stego_key key;
    key.keys = malloc(sizeof(struct stego_pixel) * max_size);
    assert(key.keys);
    load_key(&key, key_filename, max_size * 5);
    char* msg = malloc(key.size / 5 + 1);
    assert(msg);
    load_msg(msg, msg_filename, key.size / 5);
    insert(&image, &key, msg);
    save_bmp(&image, out_filename);
    free(key.keys);
    free(msg);
    return 0;
  }
  if (strcmp(argv[1], "extract") == 0) {
    assert(argc >= 5);
    char *in_filename = argv[2];
    char *key_filename = argv[3], *msg_filename = argv[4];
    load_bmp(&image, in_filename);
    int max_size = image.header.biWidth * image.header.biHeight * 3;
    struct stego_key key;
    key.keys = malloc(sizeof(struct stego_pixel) * max_size);
    assert(key.keys);
    load_key(&key, key_filename, max_size * 5);
    char* msg = malloc(key.size / 5 + 1);
    assert(msg);
    extract(&image, &key, msg);
    save_msg(msg, msg_filename);
    free_table(&image);
    free(key.keys);
    free(msg);
    return 0;
  }
  
  return 0;
}
