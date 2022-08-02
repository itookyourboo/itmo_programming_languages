#ifndef BMP_READER_H
#define BMP_READER_H

#include <stdio.h>

#include "bmp_header.h"
#include "bmp_io_status.h"
#include "bmp_padding.h"

#include "../img/image.h"

enum read_status read_bmp_header(FILE *in, struct bmp_header *header);

enum read_status read_bmp_pixel_array(FILE *in, struct image *image, struct bmp_header *header);

#endif //BMP_READER_H
