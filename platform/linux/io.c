#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include "common/logger.h"
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

char* jackc_read_file_content(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        return NULL;
    }
    LOG_DEBUG("Opened source file.\n");

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    long file_size = ftell(file);
    rewind(file);
    LOG_DEBUG("Calculated file content length.\n");

    char* content = jackc_alloc((size_t)file_size + 1);
    size_t bytes_read = fread(content, sizeof(char), (size_t)file_size, file);
    content[bytes_read] = '\0';
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        return NULL;
    }

    LOG_DEBUG("Saved the content to a buffer.\n");
    return content;
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

ssize_t jackc_write(int fd, const void* buf, size_t n) {
    return write(fd, buf, n);
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
