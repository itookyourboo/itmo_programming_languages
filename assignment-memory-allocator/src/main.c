#include <stdio.h>

#include "test.h"

// В главном файле мы запускаем наши тесты
// Если все тесты проходит, выведет TESTS PASSED
// Если нет, то TESTS FAILED

// Главные файлы:
// mem.c        реализация работы с памятью
// test.c       тесты для проверки

int main() {
    bool res = test_all();
    if (!res) {
        printf("TESTS FAILED\n");
        return 1;
    }

    printf("TESTS PASSED\n");
    return 0;
}
