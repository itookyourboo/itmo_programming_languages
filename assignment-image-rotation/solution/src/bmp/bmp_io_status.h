#ifndef BMP_IO_STATUS_H
#define BMP_IO_STATUS_H


// Здесь лежат статусы при работе с файлами
enum read_status {
    // Все ништяк
    READ_OK = 0,
    // Если файл не подписан, как BMP
    READ_INVALID_SIGNATURE,
    // Если у нас кол-во битов в строке
    // не соответствует BMP-файлу
    READ_INVALID_BITS,
    // Если корявый заголовок
    READ_INVALID_HEADER,
    // Ошибка при открытии
    READ_ERROR
};

// Тут аналогично
enum write_status {
    WRITE_OK = 0,
    WRITE_HEADER_ERROR,
    WRITE_PIXELS_ERROR,
    WRITE_ERROR
};

#endif
