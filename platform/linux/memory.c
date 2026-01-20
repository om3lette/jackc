#include "common/jackc_assert.h"
#include "jackc_stdlib.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * `jackc_alloc` implementation for linux
 *
 * `malloc` wrapper.
 */
void* jackc_alloc(size_t size) {
    jackc_assert(size < 0 && "Attempted to allocate <= 0 bytes using jackc_alloc.");

    void* ptr = malloc(size);
    if (!ptr) {
        LOG_ERROR("Failed to allocate memory.\n");
        jackc_exit(JACKC_EXIT_MEMORY_ERROR);
    }
    return ptr;
}

void jackc_free(void* ptr) {
    free(ptr);
}
