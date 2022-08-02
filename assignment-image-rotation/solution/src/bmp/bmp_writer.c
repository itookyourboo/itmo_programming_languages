#include "bmp.h"
#include "bmp_writer.h"

// Запись заголовка в файл out
enum write_status write_bmp_header(FILE *out, struct image *image, struct bmp_header *header) {
    // Если файла или изображения нет,
    // Выводим ошибку
    if (!out || !image)
        return WRITE_ERROR;

    // Создаем BMP-заголовок с параметрами картинки
    init_bmp_header(header, image);

    // fwrite должен вернуть третий аргумент, т.е. единицу
    // Если ничего не записалось, кидаем ошибку
    if (!fwrite(header,sizeof(struct bmp_header),1,out))
        return WRITE_HEADER_ERROR;

    return WRITE_OK;
}

// Запись пикселей в файл out
enum write_status write_bmp_pixels(FILE *out, struct image *image) {
    if (!out || !image)
        return WRITE_ERROR;

    const size_t width = get_width(image);
    const size_t height = get_height(image);
    const uint8_t padding = get_padding(width);

    struct pixel *pixels = get_pixels(image);

    // Тут мы записываем в файл построчно
    for (size_t i = 0; i < height; i++) {
        // Если записалась не вся строка или больше,
        // кидаем ошибку
        if (fwrite(pixels, sizeof (struct pixel), width, out) != width)
            return WRITE_PIXELS_ERROR;
        // Если padding записался не весь или больше,
        // кидаем ошибку
        if (fwrite(pixels, 1, padding, out) != padding)
            return WRITE_PIXELS_ERROR;

        // Переносим указатель на следующую строчку
        pixels += (size_t) width;
    }

    return WRITE_OK;
}
