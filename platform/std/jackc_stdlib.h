#ifndef JACKC_STDLIB_H
#define JACKC_STDLIB_H

#include <stddef.h>
#include <stdarg.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(x) ((x) < 0 ? -(x) : (x))

/**
 * Allocates `size` bytes. Exits on allocation failure.
 *
 * @param size Bytes to allocate.
 *
 * @pre size should positive (> 0).
 */
void* jackc_alloc(size_t size);

/**
 * Terminates program execution with `code` status code.
 *
 * @param code Termination status code.
 */
void jackc_exit(int code);

void jackc_free(void* ptr);

void* jackc_memcpy(void* dest, const void* src, size_t n);

void* jackc_memset(void *dest, int c, size_t n);

#endif
