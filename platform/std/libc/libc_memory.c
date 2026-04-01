#include "std/jackc_stdlib.h"
#include "core/exit_code.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * `jackc_alloc` implementation for linux
 *
 * `malloc` wrapper.
 */
void* jackc_alloc(size_t size) {
    if (size == 0) {
        jackc_exit(JACKC_EXIT_ALLOCATION_ERROR);
    }

    void* ptr = malloc(size);
    if (!ptr) {
        jackc_exit(JACKC_EXIT_ALLOCATION_ERROR);
    }
    return ptr;
}

void jackc_free(void* ptr) {
    free(ptr);
}

void* jackc_memcpy(void* dest, const void* src, size_t n) {
    return memcpy(dest, src, n);
}

void* jackc_memset(void *dest, int c, size_t n) {
    return memset(dest, c, n);
}
