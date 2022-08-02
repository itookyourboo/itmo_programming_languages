#ifndef BMP_PADDING_H
#define BMP_PADDING_H

#include <stdint.h>
#include <stdio.h>

#define DWORD_SIZE 4
#define COLOR_DEPTH 24

uint8_t get_padding(size_t width);

#endif //BMP_PADDING_H
