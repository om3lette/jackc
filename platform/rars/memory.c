#include "jack_stdlib.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include "rars/rars_syscalls.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * `jackc_alloc` implementation for linux
 *
 * `malloc` wrapper.
 */
void* jackc_alloc(size_t size) {
    assert(size > 0 && "Attempted to allocate <= 0 bytes using jackc_alloc.");

    void* ptr = rars_sbrk(size);
    if (!ptr) {
        LOG_ERROR("Failed to allocate memory");
        jackc_exit(JACKC_EXIT_MEMORY_ERROR);
    }
    return ptr;
}
