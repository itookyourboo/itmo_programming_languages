#ifndef FILE_H
#define FILE_H

#include <stdio.h>

FILE *open_file(char *name, char *mode);

void close_file(FILE *file);

#endif //FILE_H
