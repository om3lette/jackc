#ifndef JACKC_STDIO_H
#define JACKC_STDIO_H

#include <stddef.h>
#include <stdarg.h>

#define O_CREAT 0100
#define O_RDONLY 00
#define O_WRONLY 01
#define O_APPEND 0200
#define O_TRUNC 01000

void jackc_vfprintf(int fd, const char* format, va_list args);

void jackc_fprintf(int fd, const char* format, ...);

void jackc_vsprintf(char* buffer, const char* format, va_list args);

void jackc_sprintf(char* buffer, const char* format, ...);

int jackc_open(const char* path, int flags);

long jackc_write(int fd, const void* buf, size_t n);

int jackc_close(int fd);

#endif
