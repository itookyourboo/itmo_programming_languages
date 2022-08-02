#include "bmp.h"
#include "bmp_header.h"
#include "bmp_io_status.h"
#include "bmp_reader.h"


enum read_status from_bmp(FILE *in, struct image *image) {
    struct bmp_header header = {0};
    enum read_status header_read_status = read_bmp_header(in, &header);

    if (header_read_status != READ_OK) {
        return header_read_status;
    }

    set_height(image, header.biHeight);
    set_width(image, header.biWidth);

    return read_bmp_pixel_array(in, image, &header);
}

enum write_status to_bmp(FILE *out, struct image *image) {
    if (!out || !image) {
        return WRITE_ERROR;
    }

    struct bmp_header header = {0};
    init_bmp_header(&header, image);

    size_t image_width = get_width(image);
    size_t image_height = get_height(image);
    uint8_t padding = get_padding(image_width);

    struct pixel *pixel_array = get_pixel_array(image);

    fwrite(&header, sizeof(struct bmp_header), 1, out);
    fseek(out, header.bOffBits, SEEK_SET);

    uint8_t *padding_trash = calloc(1, padding);

    for (size_t i = 0; i < image_height; i++) {
        fwrite(pixel_array + header.biWidth * i, header.biWidth * sizeof(struct pixel), 1, out);
        fwrite(padding_trash, padding, 1, out);
    }

    free(padding_trash);

    return WRITE_OK;
}

void init_bmp_header(struct bmp_header *header, struct image *image) {
    size_t image_width = get_width(image);
    size_t image_height = get_height(image);
    uint8_t padding = get_padding(image_width);

    size_t image_size = (sizeof(struct pixel) * image_width + padding) * image_height;

    header->bfType = BMP_TYPE;
    header->biBitCount = COLOR_DEPTH;
    header->biXPelsPerMeter = BMP_PIXEL_PER_METER;
    header->biYPelsPerMeter = BMP_PIXEL_PER_METER;
    header->bfileSize = image_size + BMP_RESERVED + sizeof(struct bmp_header);
    header->bfReserved = BMP_RESERVED;
    header->bOffBits = sizeof(struct bmp_header);
    header->biSize = BMP_HEADER_SIZE;
    header->biWidth = image_width;
    header->biHeight = image_height;
    header->biPlanes = BMP_COMPRESSION;
    header->biCompression = BMP_COMPRESSION;
    header->biSizeImage = image_size;
    header->biClrUsed = BMP_COLORS;
    header->biClrImportant = BMP_COLORS;
}
