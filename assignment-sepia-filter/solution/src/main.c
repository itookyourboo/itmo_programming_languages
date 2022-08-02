#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "bmp/bmp.h"
#include "file/file.h"
#include "img/image.h"
#include "transform/sepia_filter.h"


static void perf_counter(char input_file[]) {
    int TESTS_NUM = 50;
    unsigned long long c_sum = 0, c_min = 999999, c_max = 0;
    unsigned long long sse_sum = 0, sse_min = 999999, sse_max = 0;

    FILE *input_bmp = open_file(input_file, "r");
    struct image *original_image = alloc_image(0, 0);
    from_bmp(input_bmp, original_image);
    close_file(input_bmp);

    for (int i = 0; i < TESTS_NUM; i++) {
        struct timeval t0, t1, t2;

        gettimeofday(&t0, 0);
        sepia_image(original_image);

        gettimeofday(&t1, 0);

        sepia_image_sse(original_image);
        gettimeofday(&t2, 0);

        unsigned long long c_res = (t1.tv_sec - t0.tv_sec) * 1000000L + t1.tv_usec - t0.tv_usec;
        unsigned long long sse_res = (t2.tv_sec - t1.tv_sec) * 1000000L + t2.tv_usec - t1.tv_usec;

        c_sum += c_res;
        if (c_res < c_min) c_min = c_res;
        if (c_res > c_max) c_max = c_res;

        sse_sum += sse_res;
        if (sse_res < sse_min) sse_min = sse_res;
        if (sse_res > sse_max) sse_max = sse_res;
    }

    free_image(original_image);

    unsigned long long c_avg = c_sum / TESTS_NUM;
    unsigned long long sse_avg = sse_sum / TESTS_NUM;

    printf("\tMIN\tAVG\tMAX\n");
    printf("C:\t%llu\t%llu\t%llu\n", c_min, c_avg, c_max);
    printf("SSE:\t%llu\t%llu\t%llu\n", sse_min, sse_avg, sse_max);
}

static void make_sepia(int sse, char input_file[], char output_file[]) {
    FILE *input_bmp = open_file(input_file, "r");
    struct image *original_image = alloc_image(0, 0);
    from_bmp(input_bmp, original_image);
    close_file(input_bmp);

    FILE *output_bmp = open_file(output_file, "w");

    struct image *filtered_image;

    if (sse) {
        filtered_image = sepia_image_sse(original_image);
    } else {
        filtered_image = sepia_image(original_image);
    }

    to_bmp(output_bmp, filtered_image);
    close_file(output_bmp);

    free_image(original_image);
    free_image(filtered_image);
}


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./sepia-filter <source-image> <filtered-image>");
        return -1;
    }

    char sse_key[] = "-sse";
    char perf_key[] = "-perf";

    if (strcmp(argv[1], perf_key) == 0) {
        perf_counter(argv[2]);
    } else {
        int sse = strcmp(argv[1], sse_key) == 0;
        size_t arg_i = 1 + sse;
        char *input_file = argv[arg_i++];
        char *output_file = argv[arg_i];

        make_sepia(sse, input_file, output_file);
    }

    return 0;
}
