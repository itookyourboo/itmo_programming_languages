#ifndef BMP_H
#define BMP_H

#define DWORD_SIZE 4
#define COLOR_DEPTH 24
#define BMP_TYPE 0x4D42
#define BMP_RESERVED 0
#define BMP_COMPRESSION 0
#define BMP_PIXEL_PER_METER 2834
#define BMP_PLANES 1
#define BMP_COLORS 0
#define BMP_HEADER_SIZE 40

#include <stdio.h>

#include "bmp_header.h"
#include "bmp_io_status.h"

#include "../img/image.h"

enum read_status from_bmp(FILE *in, struct image *image);

enum write_status to_bmp(FILE *out, struct image *image);

void init_bmp_header(struct bmp_header *header, struct image *image);

#endif //BMP_H
