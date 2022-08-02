#ifndef IMAGE_H
#define IMAGE_H

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>


// packed просто упакует его в массив
struct pixel {
    uint8_t b, g, r;
} __attribute__((packed));


struct image {
    size_t width;
    size_t height;
    struct pixel *pixels;
};


size_t get_height(struct image *image);
void set_height(struct image *image, size_t height);

size_t get_width(struct image *image);
void set_width(struct image *image, size_t width);

struct pixel *get_pixels(struct image *image);
void set_pixels(struct image *image, struct pixel *pixels);

struct pixel get_pixel(struct image *image, size_t col, size_t row);
void set_pixel(struct image *image, struct pixel pixel, size_t col, size_t row);

struct image alloc_image(size_t width, size_t height);
void free_image(struct image *image);

struct pixel *alloc_pixels(size_t width, size_t height);
void free_pixels(struct pixel *pixels);

#endif
