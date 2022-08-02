#include "rotate.h"

#include "../img/image.h"

struct image *rotate_image(struct image *input_image) {
    if (input_image == NULL) {
        return NULL;
    }

    size_t new_height = get_width(input_image);
    size_t new_width = get_height(input_image);
    struct image *output_image = alloc_image(new_width, new_height);
    for (size_t i = 0; i < new_height; i++) {
        for (size_t j = 0; j < new_width; j++) {
            struct pixel pixel = get_pixel(input_image, i, new_width - j - 1);
            set_pixel(output_image, pixel, j, i);
        }
    }
    return output_image;
}
