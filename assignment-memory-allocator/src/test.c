#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>

#include "mem.h"
#include "mem_internals.h"
#include "test.h"

void *heap;
struct block_header *head;

inline static void print_heap() {
    debug_heap(stdout, head);
}

#define HEAP_SIZE 5000

#define MALLOC_SIZE 1000

bool test1() {
    print_heap();
    void *mal = _malloc(MALLOC_SIZE);
    print_heap();
    if (mal == NULL || head->capacity.bytes != MALLOC_SIZE) {
        return false;
    }

    _free(mal);

    return true;
}

inline static int get_freed_blocks() {
    int freed = 0;
    struct block_header *next = head;
    while (next) {
        if (next->is_free) freed++;
        next = next->next;
    }

    return freed;
}

#define FIRST_BLOCK 1000
#define SECOND_BLOCK 2000

bool test2() {
    print_heap();
    void *block1 = _malloc(FIRST_BLOCK);
    void *block2 = _malloc(SECOND_BLOCK);
    print_heap();

    _free(block1);
    print_heap();

    // Так как у нас еще был исходный блок, поэтому 1 + 1
    if (get_freed_blocks() != 2) return false;

    _free(block2);
    _free(block1);

    return true;
}

#define THIRD_BLOCK 3000

bool test3() {
    print_heap();
    void *block1 = _malloc(FIRST_BLOCK);
    void *block2 = _malloc(SECOND_BLOCK);
    void *block3 = _malloc(THIRD_BLOCK);
    print_heap();

    _free(block1);
    _free(block2);
    print_heap();

    // Аналогично 2 + 1
    if (get_freed_blocks() != 3) return false;

    _free(block3);
    _free(block2);
    _free(block1);

    return true;
}

#define FIRST_BLOCK_4 7000

bool test4() {
    print_heap();
    void *block1 = _malloc(FIRST_BLOCK_4);
    void *block2 = _malloc(SECOND_BLOCK);
    print_heap();

    int allocated = 0;
    struct block_header *next = head;
    while (next->next && ++allocated) next = next->next;

    // Если у нас куча не расширилась (т.е. не выделено два блока)
    if (allocated != 2) return false;

    _free(block2);
    _free(block1);

    return true;
}

#define HUGE 50000

bool test5() {
    head->capacity.bytes = HEAP_SIZE;
    print_heap();

    struct block_header *next = head;
    while (next) next = next->next;

    // Тут сложная система, он создает память напрямую без mem.c
    size_t count = (size_t) next / getpagesize();
    size_t remains = (size_t) next % getpagesize();
    uint8_t *mem = (uint8_t * )(getpagesize() * (count + (remains > 1)));
    (void) mmap(mem, 1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    void *allocated = _malloc(HUGE);

    // Если адреса (с учетом отсутпа хедера) не совпадают
    if (mem == (uint8_t * )((uint8_t *) allocated - offsetof(struct block_header, contents))) return false;

    print_heap();
    _free(allocated);

    return true;
}

typedef bool (*test)(void);

test tests[5] = {&test1, &test2, &test3, &test4, &test5};

inline static void print_divider() {
    for (int i = 0; i < 40; i++) printf("=");
    printf("\n");
}

bool test_all() {
    heap = heap_init(HEAP_SIZE);
    head = (struct block_header *) heap;

    bool res = true;

    for (size_t i = 0; i < 5; i++) {
        print_divider();
        printf("TEST %zu\n", i + 1);
        bool res_i = tests[i]();
        if (res_i) {
            printf("\nTEST %zu PASSED\n", i + 1);
        } else {
            printf("\nTEST %zu FAILED\n", i + 1);
        }
        res &= res_i;
        print_divider();
    }

    return res;
}
