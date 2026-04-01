#include "core/exit_code.h"
#include "rars/rars_syscalls.h"
#include "std/jackc_stdlib.h"

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
    (void) ptr; // We don't need to do anything here as RARS doesn't support freeing memory
}
