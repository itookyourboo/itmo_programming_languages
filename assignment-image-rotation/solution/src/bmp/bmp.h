#ifndef BMP_H
#define BMP_H

#define TYPE 0x4D42
#define PADDING 4
#define BIT_COUNT 24
#define PPX 2834
#define FILE_SIZE
#define RESERVED 0
#define HEADER_SIZE 40
#define COMPRESSION 0
#define COLORS 0

#include <stdio.h>

#include "../img/image.h"
#include "bmp_io_status.h"


// Это заголовок файла с характеристиками BMP-изображения
struct bmp_header {
    uint16_t bfType;
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} __attribute__((packed));


enum read_status from_bmp(FILE *in, struct image *image);
enum write_status to_bmp(FILE *out, struct image *image);

enum read_status from_bmp_name(char *in, struct image *image);
enum write_status to_bmp_name(char *out, struct image *image);

void init_bmp_header(struct bmp_header *header, struct image *image);

uint8_t get_padding(size_t width);

#endif
