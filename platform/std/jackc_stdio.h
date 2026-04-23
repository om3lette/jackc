#ifndef JACKC_STDIO_H
#define JACKC_STDIO_H

#include <stddef.h>
#include <stdarg.h>
#include "jackc_syscalls.h"

#ifdef __rars__
    #define FRONTEND_EXPECTED_ARGUMENTS 2
    #define BACKEND_EXPECTED_ARGUMENTS 2
    #define JACKC_EXPECTED_ARGUMENTS 2
    #define FIRST_ARG_IDX 0
#else
    #define FRONTEND_EXPECTED_ARGUMENTS 3
    #define BACKEND_EXPECTED_ARGUMENTS 3
    #define JACKC_EXPECTED_ARGUMENTS 3
    #define FIRST_ARG_IDX 1
#endif

#ifdef _WIN32
    #define DELIMITER '\\'
#else
    #define DELIMITER '/'
#endif

void jackc_putchar(char c);

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

void jackc_vprintf(const char* format, va_list args);

void jackc_vfprintf(FD fd, const char* format, va_list args);

void jackc_fprintf(FD fd, const char* format, ...);

void jackc_vsprintf(char* buffer, const char* format, va_list args);

void jackc_sprintf(char* buffer, const char* format, ...);

[[ nodiscard ]] FD jackc_stdout_fd();

#endif
