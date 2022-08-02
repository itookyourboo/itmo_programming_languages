#include "image.h"


size_t get_height(struct image *image) {
    return image->height;
}

size_t get_width(struct image *image) {
    return image->width;
}

void set_height(struct image *image, size_t height) {
    image->height = height;
}

void set_width(struct image *image, size_t width) {
    image->width = width;
}

struct pixel get_pixel(struct image *image, size_t x_axis, size_t y_axis) {
    return image->pixels[y_axis * image->width + x_axis];
}

struct pixel *get_pixel_array(struct image *image) {
    return image->pixels;
}


void set_pixel(struct image *image, struct pixel pixel, size_t x_axis, size_t y_axis) {
    image->pixels[y_axis * image->width + x_axis] = pixel;
}

void set_pixel_array(struct image *image, struct pixel *pixels) {
    image->pixels = pixels;
}

struct image *alloc_image(size_t width, size_t height) {
    struct image *image = malloc(sizeof(struct image));
    image->width = width;
    image->height = height;
    image->pixels = alloc_pixel_array(width, height);

    return image;
}

struct pixel *alloc_pixel_array(size_t width, size_t height) {
    return malloc(width * height * sizeof(struct pixel));
}

void free_image(struct image *image) {
    free_pixel_array(image->pixels);
    free(image);
}

void free_pixel_array(struct pixel *pixels) {
    free(pixels);
}
