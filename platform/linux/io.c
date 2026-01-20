#include "jackc_stdlib.h"
#include "common/logger.h"
#include <stdarg.h>
#include <stdio.h>

void jackc_print_newline() {
    putchar('\n');
}

void jackc_print_char(char c) {
    putchar(c);
}

void jackc_print_int(int n) {
    printf("%d", n);
}

void jackc_print_float(float f) {
    printf("%f", f);
}

void jackc_print_string(const char *str) {
    printf("%s", str);
}

char* jackc_read_file_content(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        return NULL;
    }
    LOG_DEBUG("Opened source file.");

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    long file_size = ftell(file);
    rewind(file);
    LOG_DEBUG("Calculated file content length.");

    char* content = jackc_alloc((size_t)file_size + 1);
    size_t bytes_read = fread(content, sizeof(char), (size_t)file_size, file);
    content[bytes_read] = '\0';
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        return NULL;
    }

    LOG_DEBUG("Saved the content to a buffer.");
    return content;
}
