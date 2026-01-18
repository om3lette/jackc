#ifndef JACK_STDLIB_H
#define JACK_STDLIB_H

#include <stddef.h>

/**
 * Opens a file by given path, reads file content, returns a pointer to a buffer.
 *
 * Buffer is allocated inside of a function.
 *
 * @param path Path to file.
 */
char* jackc_read_file_content(const char* path);

/**
 * Mirrors printf behavior.
 *
 * @param format Format string.
 */

void jackc_print_newline();

void jackc_print_int(int n);

void jackc_print_float(float f);

void jackc_print_string(const char* str);

void jackc_print_char(char c);

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

#endif
