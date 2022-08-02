#include "rotate.h"
#include "../img/image.h"

// Тут обычный алгоритм на поворот массива
// Создаем транспонированное изображение
// Тогда пиксель (x, y) полученного есть
// (y, height - 1 - x) исходного
struct image rotate_image(struct image source) {
    const size_t dest_height = get_width(&source);
    const size_t dest_width = get_height(&source);

    struct image dest = alloc_image(dest_width, dest_height);

    for (size_t i = 0; i < dest_height; i++) {
        for (size_t j = 0; j < dest_width; j++) {
            struct pixel pixel = get_pixel(&source, i, dest_width - j - 1);
            set_pixel(&dest, pixel, j, i);
        }
    }

    return dest;
}
