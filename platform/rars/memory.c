#include "jackc_stdlib.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include "rars/rars_syscalls.h"
#include "common/jackc_assert.h"
#include <stddef.h>

/**
 * `jackc_alloc` implementation for rars
 *
 * `sbrk` syscall wrapper.
 */
void* jackc_alloc(size_t size) {
    jackc_assert(size > 0 && "Attempted to allocate <= 0 bytes using jackc_alloc.");

    void* ptr = rars_sbrk(size);
    if (!ptr) {
        LOG_ERROR("Failed to allocate memory.\n");
        jackc_exit(JACKC_EXIT_MEMORY_ERROR);
    }
    return ptr;
}

void jackc_free(void* ptr) {
    (void) ptr; // We don't need to do anything here as rars doesn't support freeing memory
}

// GCC can add implicit calls to memcpy. Not meant to be used directly.
__attribute__((weak)) void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}
