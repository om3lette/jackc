#ifndef JACKC_STDIO_H
#define JACKC_STDIO_H

#include <stddef.h>
#include <stdarg.h>

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

void jackc_vfprintf(int fd, const char* format, va_list args);

void jackc_fprintf(int fd, const char* format, ...);

void jackc_vsprintf(char* buffer, const char* format, va_list args);

void jackc_sprintf(char* buffer, const char* format, ...);

int jackc_open(const char* path, int flags);

long jackc_read(int fd, void* buf, size_t n);

long jackc_lseek(int fd, long offset, int whence);

long jackc_write(int fd, const void* buf, size_t n);

int jackc_close(int fd);

#endif
