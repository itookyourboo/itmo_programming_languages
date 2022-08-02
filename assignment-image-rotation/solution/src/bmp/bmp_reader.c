#include "bmp.h"
#include "bmp_reader.h"

enum read_status read_bmp_header(FILE *in, struct bmp_header *header) {
    if (!in || !header)
        return READ_ERROR;

    if (!fread(header, sizeof(struct bmp_header), 1, in))
        return READ_ERROR;

    if (header->bfType != TYPE)
        return READ_INVALID_SIGNATURE;

    if (header->bfileSize != header->bOffBits + header->biSizeImage)
        return READ_INVALID_HEADER;

    return READ_OK;
}

enum read_status read_bmp_pixels(FILE *in, struct image *image, struct bmp_header *header) {
    const size_t img_width = header->biWidth;
    const size_t img_height = header->biHeight;
    const uint8_t padding = get_padding(img_width);

    struct pixel const *pixels = alloc_pixels(img_width, img_height);

    if (fseek(in, header->bOffBits, SEEK_SET))
        return READ_ERROR;

    for (size_t i = 0; i < img_height; i++) {
        if (fread(
                pixels + i * img_width,
                sizeof(struct pixel),
                img_width,
                in
        ) != img_width) {
            free_pixels(get_pixels(image));
            return READ_INVALID_BITS;
        }

        if (fseek(in, padding, SEEK_CUR)) {
            free_pixels(get_pixels(image));
            return READ_INVALID_BITS;
        }
    }

    free_pixels(get_pixels(image));
    set_pixels(image, pixels);

    return READ_OK;
}
