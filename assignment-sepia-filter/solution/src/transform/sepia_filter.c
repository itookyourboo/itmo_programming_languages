#include "sepia_filter.h"

#include "../img/image.h"

static float pixel_array_float[256];

static void init_pixel_array() {
    if (!pixel_array_float[1]) {
        for (size_t i = 0; i < 256; i++) {
            pixel_array_float[i] = (float) i;
        }
    }
}

static const float c[3][3] = {
        {.393f, .769f, .189f},
        {.349f, .686f, .168f},
        {.272f, .543f, .131f}
};

static unsigned char sat(uint64_t x) {
    if (x < 256) return x;
    return 255;
}

void sepia_filter_sse(float *, float *, float *, void *);

static void sepia_one(struct pixel *const input_pixel, struct pixel *const output_pixel) {
    const float R = pixel_array_float[input_pixel->r],
            G = pixel_array_float[input_pixel->g],
            B = pixel_array_float[input_pixel->b];

    output_pixel->r = sat((uint64_t)(
            R * c[0][0] +
            G * c[0][1] +
            B * c[0][2]
    ));
    output_pixel->g = sat((uint64_t)(
            R * c[1][0] +
            G * c[1][1] +
            B * c[1][2]
    ));
    output_pixel->b = sat((uint64_t)(
            R * c[2][0] +
            G * c[2][1] +
            B * c[2][2]
    ));
}

static struct pixel *pixel_of(struct image *input_image, uint32_t index) {
    return &input_image->pixels[index];
}

struct image *sepia_image(struct image *input_image) {
    init_pixel_array();
    struct image *output_image = alloc_image(input_image->width, input_image->height);

    for (size_t i = 0; i < output_image->width * output_image->height; i++)
        sepia_one(
                pixel_of(input_image, i),
                pixel_of(output_image, i)
                );

    return output_image;
}

struct image *sepia_image_sse(struct image *input_image) {
    init_pixel_array();
    float b[4], g[4], r[4];

    struct image *output_image = alloc_image(input_image->width, input_image->height);

    const size_t pixels_num = input_image->width * input_image->height;
    const size_t pixels_remains = pixels_num % 4;

    struct pixel *input_pix;

    for (size_t i = 0; i < pixels_num - pixels_remains; i += 4) {
        for (size_t k = 0; k < 4; k++) {
            input_pix = pixel_of(input_image, i + k);
            b[k] = pixel_array_float[input_pix->b];
            g[k] = pixel_array_float[input_pix->g];
            r[k] = pixel_array_float[input_pix->r];
        }

        sepia_filter_sse(r, g, b, pixel_of(output_image, i));
    }

    for (size_t i = pixels_num - pixels_remains; i < pixels_num; i++) {
        sepia_one(
                pixel_of(input_image, i),
                pixel_of(output_image, i)
        );
    }

    return output_image;
}
