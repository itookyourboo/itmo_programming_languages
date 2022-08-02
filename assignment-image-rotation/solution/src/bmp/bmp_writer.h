#ifndef BMP_WRITER_H
#define BMP_WRITER_H

#include <stdio.h>

#include "bmp.h"
#include "bmp_io_status.h"

#include "../img/image.h"

enum write_status write_bmp_header(FILE *out, struct image *image, struct bmp_header *header);
enum write_status write_bmp_pixels(FILE *out, struct image *image);

#endif
