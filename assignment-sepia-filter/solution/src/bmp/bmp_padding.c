#include "bmp_padding.h"

uint8_t get_padding(size_t width) {
    return DWORD_SIZE - (width * (COLOR_DEPTH / 8)) % DWORD_SIZE;
}
