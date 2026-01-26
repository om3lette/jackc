#ifndef JACKC_STDLIB_H
#define JACKC_STDLIB_H

#include <stddef.h>
#include <stdarg.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

/**
 * Opens a file by given path, reads file content, returns a pointer to a buffer.
 *
 * Buffer is allocated inside of a function.
 *
 * @param path Path to file.
 */
char* jackc_read_file_content(const char* path);

/**
 * Tries to mirror printf behavior.
 *
 * Currently supports (RARS):
 * - %d: integer
 * - %u: unsigned integer
 * - %c: char
 * - %s: string
 * - %f: float
 * - %p: pointer
 *
 * @param format Format string.
 */
void jackc_printf(const char* format, ...);

void jackc_putchar(char c);

void jackc_vprintf(const char* format, va_list args);

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

#endif
