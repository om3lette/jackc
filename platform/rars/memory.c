#include "jackc_stdlib.h"
#include "core/exit_code.h"
#include "rars/rars_syscalls.h"
#include <stddef.h>

/**
 * `jackc_alloc` implementation for rars
 *
 * `sbrk` syscall wrapper.
 */
void* jackc_alloc(size_t size) {
    if (size == 0) {
        jackc_exit(JACKC_EXIT_ALLOCATION_ERROR);
    }

    void* ptr = rars_sbrk(size);
    if (!ptr) {
        jackc_exit(JACKC_EXIT_ALLOCATION_ERROR);
    }
    return ptr;
}

void jackc_free(void* ptr) {
    (void) ptr; // We don't need to do anything here as rars doesn't support freeing memory
}

void* jackc_memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

// GCC can add implicit calls to memcpy. Not meant to be used directly.
__attribute__((weak)) void* memcpy(void* dest, const void* src, size_t n) {
    return jackc_memcpy(dest, src, n);
}
