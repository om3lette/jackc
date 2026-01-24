#include "jackc_stdlib.h"
#include "common/logger.h"
#include <stdarg.h>
#include <stdio.h>

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
