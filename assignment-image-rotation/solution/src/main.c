#include "bmp/bmp.h"
#include "img/image.h"
#include "op/rotate.h"
#include <stdio.h>


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./image-transformer <source-image> <transformed-image>");
        return -1;
    }

    // Создаем пустую картинку
    struct image source = { 0 };
    // Читаем в нее данные из файла по имени argv[1]
    enum read_status rs = from_bmp_name(argv[1], &source);

    if (rs != READ_OK) {
        fprintf(stderr, "COULD NOT READ BMP, ERROR: %d\n", rs);
        free_image(&source);
        return rs;
    }

    int w_status = 0;

    // rotate_image уже вернет готовый image
    struct image dest = rotate_image(source);
    // останется его только записать в файл по имени argv[2]
    enum write_status ws = to_bmp_name(argv[2], &dest);

    if (ws != WRITE_OK) {
        fprintf(stderr, "COULD NOT WRITE BMP, ERROR: %d\n", ws);
        w_status = ws;
    }

    // освобождаем память, которую мы использовали
    // для картинок (точнее для массивов пикселей)
    free_image(&source);
    free_image(&dest);

    // Вернет 0 (WRITE_OK) в случае успешного завершения
    // Или номер ошибки > 0 в случае неудачи
    return w_status;
}
