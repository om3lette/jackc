#include "jackc_stdlib.h"
#include "common/logger.h"
#include "rars/rars_syscalls.h"

void jackc_print_newline() {
    jackc_print_char(10);
}

void jackc_print_char(char c) {
    register char a0_c asm("a0") = c;
    register int a7_syscall_num asm("a7") = 11;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_c), "r"(a7_syscall_num)
        : "memory"
    );
}

void jackc_print_int(int n) {
    register int a0_n asm("a0") = n;
    register int a7_syscall_num asm("a7") = 1;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_n), "r"(a7_syscall_num)
        : "memory"
    );
}

void jackc_print_float(float f) {
    register float a0_f asm("a0") = f;
    register int a7_syscall_num asm("a7") = 2;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_f), "r"(a7_syscall_num)
        : "memory"
    );
}

void jackc_print_string(const char* str) {
    register const char* a0_str asm("a0") = str;
    register int a7_syscall_num asm("a7") = 4;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_str), "r"(a7_syscall_num)
        : "memory"
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

    long file_size = rars_lseek(fd, 0L, RARS_SEEK_END);
    if (file_size < 0) {
        rars_close_file(fd);
        return NULL;
    }
    if (rars_lseek(fd, 0L, RARS_SEEK_SET) < 0) {
        rars_close_file(fd);
        return NULL;
    }
    LOG_DEBUG("Calculated file content length.");

    char* content_buffer = jackc_alloc((size_t)(file_size + 1));

    long bytes_read = rars_read(fd, content_buffer, (size_t)file_size);
    content_buffer[bytes_read] = '\0';
    rars_close_file(fd);

    if (bytes_read != file_size) {
        LOG_ERROR("Invalid size");
        return NULL;
    }

    LOG_DEBUG("Saved the content to a buffer.");
    return content_buffer;
}
