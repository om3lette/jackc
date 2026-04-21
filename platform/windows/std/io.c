#include "std/jackc_syscalls.h"

#include <consoleapi.h>
#include <processenv.h>
#include <stdarg.h>
#include <stdio.h>
#include <winbase.h>

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
    char buf[4096];
    vsnprintf(buf, sizeof(buf), format, args);
    jackc_write(fd, buf, strlen(buf));
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
