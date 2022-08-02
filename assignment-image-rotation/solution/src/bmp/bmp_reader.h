#ifndef BMP_READER_H
#define BMP_READER_H

#include <stdio.h>

#include "../img/image.h"

#include "bmp.h"
#include "bmp_io_status.h"

enum read_status read_bmp_header(FILE *in, struct bmp_header *header);
enum read_status read_bmp_pixels(FILE *in, struct image *image, struct bmp_header *header);

#endif
