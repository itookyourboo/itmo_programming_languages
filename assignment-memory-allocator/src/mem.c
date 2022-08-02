#define _DEFAULT_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

extern inline block_size
size_from_capacity( block_capacity
cap );
extern inline block_capacity
capacity_from_size( block_size
sz );

static bool block_is_big_enough(size_t query, struct block_header *block) { return block->capacity.bytes >= query; }

static size_t pages_count(size_t mem) { return mem / getpagesize() + ((mem % getpagesize()) > 0); }

static size_t round_pages(size_t mem) { return getpagesize() * pages_count(mem); }

static void block_init(void *restrict addr, block_size block_sz, void *restrict next) {
    *((struct block_header *) addr) = (struct block_header) {
            .next = next,
            .capacity = capacity_from_size(block_sz),
            .is_free = true
    };
}

static size_t region_actual_size(size_t query) { return size_max(round_pages(query), REGION_MIN_SIZE); }

extern inline bool

region_is_invalid(const struct region *r);


static void *map_pages(void const *addr, size_t length, int additional_flags) {
    return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , -1, 0 );
}

/*  аллоцировать регион памяти и инициализировать его блоком */
static struct region alloc_region(void const *addr, size_t query) {
    const block_size size = size_from_capacity( (block_capacity) { query });
    // Размер региона не должен быть меньше REGION_MIN_SIZE
    const size_t region_size = region_actual_size(size.bytes);

    // Выделяем память, равную размеру региона памяти
    void *region_address = map_pages(addr, region_size, MAP_FIXED);

    // Если не получилось, пытаемся выделить не по конкретному адресу addr
    if (region_address == NULL || region_address == MAP_FAILED)
        region_address = map_pages(addr, region_size, 0);

    // Если опять не получилось, то возвращаем ошибочку
    if (region_address == NULL || region_address == MAP_FAILED)
        return REGION_INVALID;

    // Инициализируем наш блок памяти в регионе
    block_init(region_address, (block_size) { region_size }, NULL);

    return (struct region) {
            .addr = region_address,
            .size = region_size,
            .extends = region_address == addr
    };
}

static void *block_after(struct block_header const *block);

void *heap_init(size_t initial) {
    // Пытаемся выделить память по адресу HEAP_START размером initial
    const struct region region = alloc_region(HEAP_START, initial);
    if (region_is_invalid(&region)) return NULL;

    return region.addr;
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable(struct block_header *restrict block, size_t query) {
    // Блоки можно разделить, если
    // 1) Он не занят
    // 2) Если выделив query байт, мы не выйдем за его границы
    return block->is_free &&
    query + offsetof(struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

static bool split_if_too_big(struct block_header *block, size_t query) {
    // Если нельзя разделить, то false
    if (!block_splittable(block, query)) {
        return false;
    }

    const block_capacity previous_block_capacity = {
            .bytes = query
    };

    // Так откусываем от блока query байт
    const block_size next_block_size = {
            .bytes = block->capacity.bytes - query
    };

    // Тут считаем адрес следующего блока и создаем его
    void* split_address = ((uint8_t*) block + query + offsetof(struct block_header, contents));
    block_init(split_address, next_block_size, block->next);
    block->capacity = previous_block_capacity;
    block->next = split_address;

    return true;
}


/*  --- Слияние соседних свободных блоков --- */

static void *block_after(struct block_header const *block) {
    return (void *) (block->contents + block->capacity.bytes);
}

static bool blocks_continuous(
        struct block_header const *fst,
        struct block_header const *snd) {
    return (void *) snd == block_after(fst);
}

static bool mergeable(struct block_header const *restrict fst, struct block_header const *restrict snd) {
    // Блок можно объединить со следующим если:
    // 1) Оба блока свободны
    // 2) Они стоят рядом
    return fst->is_free && snd->is_free && blocks_continuous(fst, snd);
}

static bool try_merge_with_next(struct block_header *block) {
    struct block_header *next = block->next;

    // Если следующего блока нет или они не мержутся, то false
    if (!next || !mergeable(block, next))
        return false;

    // Сливаем два блока воедино
    block->next = next->next;
    block->capacity.bytes += size_from_capacity(next->capacity).bytes;

    return true;
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
    enum {
        BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED
    } type;
    struct block_header *block;
};


static struct block_search_result find_good_or_last(struct block_header *restrict block, size_t sz) {
    // sz должен быть не меньше BLOCK_MIN_CAPACITY
    sz = size_max(sz, BLOCK_MIN_CAPACITY);
    struct block_header *current = block;
    struct block_header *found = NULL;

    // Пока у нас есть, что искать
    while(current) {
        // Пробуем смержить текущий блок со следующим
        while (try_merge_with_next(current));

        // Если размер блока не меньше запрошенного и он свободен,
        // то возвращаем его
        if (block_is_big_enough(sz, current) && current->is_free)
            return (struct block_search_result) {.type = BSR_FOUND_GOOD_BLOCK, .block = current};

        found = current;
        current = current->next;
    }

    // Если мы такого хорошего не нашли, возвращаем последний блок
    return (struct block_search_result) {
            .type = BSR_REACHED_END_NOT_FOUND,
            .block = found
    };
}

/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing(size_t query, struct block_header *block) {
    query = size_max(query, BLOCK_MIN_CAPACITY);

    // Если нашли хороший блок, то занимаем его
    // при этом если он большой, то дробим на маленькие
    struct block_search_result result = find_good_or_last(block, query);
    if (result.type == BSR_FOUND_GOOD_BLOCK) {
        split_if_too_big(result.block, query);
        result.block->is_free = false;
    }
    return result;
}


static struct block_header *grow_heap(struct block_header *restrict last, size_t query) {
    query = size_max(query, BLOCK_MIN_CAPACITY);
    // Расширяем кучу
    last->next = alloc_region(block_after(last), query + offsetof(struct block_header, contents)).addr;
    // Если можно смержить со следующим, то возвращаем большой блок
    if (try_merge_with_next(last)) {
        return last;
    }

    // Если нет, то просто возвращаем расширенную кучу
    return last->next;
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header *memalloc(size_t query, struct block_header *heap_start) {
    // Пробуем выделить блок
    struct block_search_result existing = try_memalloc_existing(query, heap_start);

    // Если не нашлось, то NULL
    if (existing.type == BSR_CORRUPTED) {
        return NULL;
    }

    // Если вернулся последний блок
    if (existing.type == BSR_REACHED_END_NOT_FOUND) {
        // Расширяем кучу
        grow_heap(existing.block, query);
        // И пробуем заново найти хороший блок
        existing = try_memalloc_existing(query, heap_start);

        // Если снова не нашлось, то NULL
        if (existing.type != BSR_FOUND_GOOD_BLOCK) {
            return NULL;
        }
    }

    return existing.block;
}

void *_malloc(size_t query) {
    struct block_header *const addr = memalloc(query, (struct block_header *) HEAP_START);
    if (addr) return addr->contents;
    else return NULL;
}

struct block_header *block_get_header(void *contents) {
    return (struct block_header *) (((uint8_t *) contents) - offsetof(struct block_header, contents));
}

void _free(void *mem) {
    // Если адрес NULL, то ничего не делаем
    if (!mem) return;
    // Ставим ему статус free
    struct block_header *header = block_get_header(mem);
    header->is_free = true;
    // И смотрим, можно ли его смержить со следующими
    while (header != NULL) {
        try_merge_with_next(header);
        header = header->next;
    }
}
