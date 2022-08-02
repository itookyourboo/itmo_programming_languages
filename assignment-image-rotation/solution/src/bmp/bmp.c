#include "bmp.h"
#include "bmp_reader.h"
#include "bmp_writer.h"


// Прочитать файл in и положить данные в image
enum read_status from_bmp(FILE *in, struct image *image) {
    // Создаём пустой заголовок
    struct bmp_header header = {0};
    // Читаем заголовок файла in в header
	enum read_status rs = read_bmp_header(in, &header);
    // Если какие-то проблемы, сразу возвращаем ошибку
	if (rs != READ_OK) return rs;

    // Если все ОК, ставим изображению ширину и высоту
	set_height(image, header.biHeight);
	set_width(image, header.biWidth);

    // Возвращаем статус при чтении массива пикселей
    // из файла in в image
	return read_bmp_pixels(in, image, &header);
}

// Аналогично с выводом данных image в out
enum write_status to_bmp(FILE *out, struct image *image) {
	struct bmp_header header = {0};

    enum write_status w_status = WRITE_OK;

    // Возвращаем ошибку, если проблемы с заголовком
    w_status = write_bmp_header(out, image, &header);
    if (w_status != WRITE_OK)
        return w_status;

    // Возвращаем ошибку, если проблемы с пикселями
    w_status = write_bmp_pixels(out, image);
    if (w_status != WRITE_OK)
        return w_status;

    // Иначе все ОК
    return w_status;
}


// Это простые обёртки над from_bmp и to_bmp
// Чтобы напрямую не работать с файлами, мы
// извлекаем изображение по имени файла
enum read_status from_bmp_name(char *in, struct image *image) {
    FILE *source_bmp = fopen(in, "r");
    const enum read_status rs = from_bmp(source_bmp, image);
    fclose(source_bmp);
    return rs;
}

enum write_status to_bmp_name(char *out, struct image *image) {
    FILE *dest_bmp = fopen(out, "w");
    const enum write_status ws = to_bmp(dest_bmp, image);
    fclose(dest_bmp);
    return ws;
}


// Тут мы инициализируем заголовок изображения
void init_bmp_header(struct bmp_header *header, struct image *image) {
    const size_t width = get_width(image);
    const size_t height = get_height(image);
    const uint8_t padding = get_padding(width);

    // padding - это пустое пространство, которое
    // дополнит ширину до размера, кратного четырем байтам
    // Так устроен BMP-файл
    const size_t size = sizeof(struct pixel) * (width + padding) * height;

    header->bfType = TYPE;
    header->biBitCount = BIT_COUNT;
    header->biXPelsPerMeter = PPX;
    header->biYPelsPerMeter = PPX;
    header->bfileSize = size + RESERVED + sizeof(struct bmp_header);
    header->bfReserved = RESERVED;
    header->bOffBits = sizeof(struct bmp_header);
    header->biSize = HEADER_SIZE;
    header->biWidth = width;
    header->biHeight = height;
    header->biPlanes = COMPRESSION;
    header->biCompression = COMPRESSION;
    header->biSizeImage = size;
    header->biClrUsed = COLORS;
    header->biClrImportant = COLORS;
}

// Функция, которая считает это пустое пространство
// Допустим, ширина у нас 15 пикселей
// Каждый пиксель занимает 3 байт (R, G, B)
// Итого в строке будет 45 байтов
// Поэтому нам надо дополнить его до кратного четырем
// 4 - 45 % 4 = 3
// 45 + 3 = 48      - порядок!
uint8_t get_padding(size_t width) {
    return PADDING - (width * (BIT_COUNT / 8)) % PADDING;
}
