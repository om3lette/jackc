#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void jackc_putchar(char c) {
    putchar(c);
}

void jackc_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void jackc_vprintf(const char* format, va_list args) {
    vprintf(format, args);
}

void jackc_vfprintf(int fd, const char* format, va_list args) {
    vdprintf(fd, format, args);
}

void jackc_fprintf(int fd, const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vfprintf(fd, format, args);
    va_end(args);
}

int jackc_open(const char* path, int flags) {
    return open(path, flags, 0644);
}

long jackc_read(int fd, void* buf, size_t n) {
    return read(fd, buf, n);
}

long jackc_write(int fd, const void* buf, size_t n) {
    return write(fd, buf, n);
}

int jackc_close(int fd) {
    return close(fd);
}

long jackc_lseek(int fd, long offset, int whence) {
    return lseek(fd, offset, whence);
}

void jackc_vsprintf(char* buffer, const char* format, va_list args) {
    vsprintf(buffer, format, args);
}

void jackc_sprintf(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vsprintf(buffer, format, args);
    va_end(args);
}
