#include "bmp_reader.h"

enum read_status read_bmp_header(FILE *in, struct bmp_header *header) {
    rewind(in);
    fread(header, sizeof(struct bmp_header), 1, in);

    return READ_OK;
}

enum read_status read_bmp_pixel_array(FILE *in, struct image *image, struct bmp_header *header) {
    size_t img_width = header->biWidth;
    size_t img_height = header->biHeight;
    uint8_t padding = get_padding(img_width);

    struct pixel *pixel_array = alloc_pixel_array(img_width, img_height);

    rewind(in);
    fseek(in, header->bOffBits, SEEK_SET);

    for (size_t i = 0; i < img_height; i++) {
        fread(pixel_array + i * img_width, sizeof(struct pixel) * img_width, 1, in);
        fseek(in, padding, SEEK_CUR);
    }

    free_pixel_array(get_pixel_array(image));
    set_pixel_array(image, pixel_array);

    return READ_OK;
}
