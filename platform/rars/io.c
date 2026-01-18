#include "jack_stdlib.h"
#include "common/logger.h"
#include "rars/rars_syscalls.h"
#include <stdarg.h>
#include <stdio.h>

void jackc_print_newline() {
    jackc_print_char(10);
}

void jackc_print_char(char c) {
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a7, 11\n\t"
        "ecall"
        :
        : "r" (c)
        : "a0", "a7"
    );
}

void jackc_print_int(int n) {
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a7, 1\n\t"
        "ecall"
        :
        : "r" (n)
        : "a0", "a7"
    );
}

void jackc_print_float(float f) {
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a7, 2\n\t"
        "ecall"
        :
        : "r" (f)
        : "a0", "a7"
    );
}

void jackc_print_string(const char *str) {
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a7, 4\n\t"
        "ecall"
        :
        : "r" (str)
        : "a0", "a7"
    );
}

/**
 * @todo: Normalize variable names accross platforms
 */
char* jackc_read_file_content(const char* file_path) {
    int fd = rars_open_file(file_path, READ_ONLY_MODE);
    if (fd < 0) {
        return NULL;
    }
    LOG_DEBUG("Opened source file.");
    LOG_DEBUG(file_path);

    long file_size = rars_lseek(fd, SEEK_END);
    if (file_size < 0) {
        rars_close_file(fd);
        return NULL;
    }
    rars_lseek(fd, SEEK_SET);
    LOG_DEBUG("Calculated file content length.");
    jackc_print_string("File size: ");
    jackc_print_int((int)file_size / 1024);
    jackc_print_string(" KB");
    jackc_print_newline();

    char* content_buffer = jackc_alloc((size_t)file_size + 1);
    long bytes_read = rars_read(fd, content_buffer, (size_t)file_size);
    content_buffer[bytes_read] = '\0';
    rars_close_file(fd);

    if (bytes_read != file_size) {
        return NULL;
    }

    LOG_DEBUG("Saved the content to a buffer.");
    return content_buffer;
}
