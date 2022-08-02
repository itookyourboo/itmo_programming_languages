#include "file.h"

FILE *open_file(char *name, char *mode) {
    return fopen(name, mode);
}

void close_file(FILE *file) {
    fclose(file);
}
