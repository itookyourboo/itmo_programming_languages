#ifndef IMAGE_H
#define IMAGE_H

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>


struct pixel {
    uint8_t b, g, r;
};


struct image {
    size_t width;
    size_t height;
    struct pixel *pixels;
};


size_t get_height(struct image *image);

void set_height(struct image *image, size_t height);

size_t get_width(struct image *image);

void set_width(struct image *image, size_t width);


struct pixel *get_pixel_array(struct image *image);

void set_pixel_array(struct image *image, struct pixel *pixels);

struct pixel get_pixel(struct image *image, size_t x_axis, size_t y_axis);

void set_pixel(struct image *image, struct pixel pixel, size_t x_axis, size_t y_axis);


struct image *alloc_image(size_t width, size_t height);

void free_image(struct image *image);

struct pixel *alloc_pixel_array(size_t width, size_t height);

void free_pixel_array(struct pixel *pixels);

#endif //IMAGE_H
