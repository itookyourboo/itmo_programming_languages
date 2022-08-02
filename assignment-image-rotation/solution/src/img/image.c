#include "image.h"


size_t get_height(struct image *image) {
    return image->height;
}

void set_height(struct image *image, size_t height) {
    image->height = height;
}

size_t get_width(struct image *image) {
    return image->width;
}

void set_width(struct image *image, size_t width) {
    image->width = width;
}

struct pixel get_pixel(struct image *image, size_t col, size_t row) {
    return image->pixels[row * image->width + col];
}

void set_pixel(struct image *image, struct pixel pixel, size_t col, size_t row) {
    image->pixels[row * image->width + col] = pixel;
}

struct pixel *get_pixels(struct image *image) {
    return image->pixels;
}

void set_pixels(struct image *image, struct pixel *pixels) {
    image->pixels = pixels;
}

struct image alloc_image(size_t width, size_t height) {
    // Изображение маленькое, поэтому создаем его в стеке
    struct image image = { 0 };
    image.width = width;
    image.height = height;
    // А вот массив пикселей большой, поэтому его создаем в куче
    image.pixels = alloc_pixels(width, height);

    return image;
}

void free_image(struct image *image) {
    free_pixels(image->pixels);
}

struct pixel *alloc_pixels(size_t width, size_t height) {
    // Так как массив пикселей довольно большой,
    // просто так в стеке его не выделишь, поэтому
    // выделяем место в куче
    return malloc(width * height * sizeof(struct pixel));
}

void free_pixels(struct pixel *pixels) {
    free(pixels);
}
