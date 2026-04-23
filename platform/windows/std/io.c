#include "std/jackc_limits.h"
#include "std/jackc_string.h"
#include "std/jackc_syscalls.h"

#include <stdarg.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

void jackc_vfprintf(FD fd, const char* format, va_list args) {
    char buf[PATH_MAX];
    vsnprintf(buf, sizeof(buf), format, args);
    jackc_write(fd, buf, jackc_strlen(buf));
}

void jackc_fprintf(FD fd, const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vfprintf(fd, format, args);
    va_end(args);
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

FD jackc_stdout_fd() {
    return (FD)GetStdHandle(STD_OUTPUT_HANDLE);
}
