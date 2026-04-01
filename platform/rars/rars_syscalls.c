#include "rars_syscalls.h"

int rars_open_file(const char* path, file_mode mode) {
    register const char* a0_path asm("a0") = path;
    register file_mode a1_mode asm("a1") = mode;
    register int a7_syscall_num asm("a7") = 1024;

    register int a0_ret asm("a0");

    __asm__ volatile (
        "ecall"
        : "=r"(a0_ret)
        : "r"(a0_path), "r"(a1_mode), "r"(a7_syscall_num)
        : "memory"
    );
    return a0_ret;
}

void rars_close_file(int fd) {
    register int a0_fd asm("a0") = fd;
    register int a7_syscall_num asm("a7") = 57;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_fd), "r"(a7_syscall_num)
        : "memory"
    );
    return;
}

long rars_lseek(int fd, long offset, int whence) {
    register int a0_fd asm("a0") = fd;
    register long a1_offset asm("a1") = offset;
    register int a2_whence asm("a2") = whence;
    register int a7_syscall_num asm("a7") = 62;

    register long a0_ret asm("a0");

    __asm__ volatile (
        "ecall"
        : "=r"(a0_ret)
        : "r"(a0_fd), "r"(a1_offset), "r"(a2_whence), "r"(a7_syscall_num)
        : "memory"
    );
    return a0_ret;
}

/**
 * RARS Read syscall wrapper impl.
 *
 * Inputs
 *
 * - `a0` = the file descriptor
 * - `a1` = address of the buffer
 * - `a2` = maximum length to read
 *
 * Outputs
 *
 * - `a0` = the length read or -1 if error
 */
long rars_read(int fd, void* buf, size_t nbytes) {
    register int a0_fd asm("a0") = fd;
    register void* a1_buf asm("a1") = buf;
    register size_t a2_nbytes asm("a2") = nbytes;
    register int a7_syscall_num asm("a7") = 63;

    register long a0_ret asm("a0");

    __asm__ volatile (
        "ecall"
        : "=r"(a0_ret)
        : "r"(a0_fd), "r"(a1_buf), "r"(a2_nbytes), "r"(a7_syscall_num)
        : "memory"
    );
    return a0_ret;
}

long rars_write(int fd, const void* buf, size_t nbytes) {
    register int a0_fd asm("a0") = fd;
    register const void* a1_buf asm("a1") = buf;
    register size_t a2_buf asm("a2") = nbytes;
    register int a7_syscall_num asm("a7") = 64;

    register long a0_ret asm("a0");

    __asm__ volatile (
        "ecall"
        : "=r"(a0_ret)
        : "r"(a0_fd), "r"(a1_buf), "r"(a2_buf), "r"(a7_syscall_num)
        : "memory"
    );
    return a0_ret;
}

void* rars_sbrk(size_t nbytes) {
    register size_t a0_nbytes asm("a0") = nbytes;
    register int a7_syscall_num asm("a7") = 9;

    register void* a0_ret asm("a0");

    __asm__ volatile (
        "ecall"
        : "=r"(a0_ret)
        : "r"(a0_nbytes), "r"(a7_syscall_num)
        : "memory"
    );
    return a0_ret;
}

void rars_exit2(int code) {
    register int a0_code asm("a0") = code;
    register int a7_syscall_num asm("a7") = 93;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_code), "r"(a7_syscall_num)
        : "memory"
    );
    return;
}

void rars_print_char(char c) {
    register char a0_c asm("a0") = c;
    register int a7_syscall_num asm("a7") = 11;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_c), "r"(a7_syscall_num)
        : "memory"
    );
}

void rars_print_int(int n) {
    register int a0_n asm("a0") = n;
    register int a7_syscall_num asm("a7") = 1;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_n), "r"(a7_syscall_num)
        : "memory"
    );
}

void rars_print_uint(unsigned int n) {
    register unsigned int a0_n asm("a0") = n;
    register int a7_syscall_num asm("a7") = 36;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_n), "r"(a7_syscall_num)
        : "memory"
    );
}

void rars_print_float(float f) {
    register float a0_f asm("a0") = f;
    register int a7_syscall_num asm("a7") = 2;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_f), "r"(a7_syscall_num)
        : "memory"
    );
}

void rars_print_string(const char* str) {
    register const char* a0_str asm("a0") = str;
    register int a7_syscall_num asm("a7") = 4;

    __asm__ volatile (
        "ecall"
        :
        : "r"(a0_str), "r"(a7_syscall_num)
        : "memory"
    );
}
