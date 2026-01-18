#include "rars_syscalls.h"

int rars_open_file(const char* path, file_mode mode) {
    int fd = -1;
    __asm__ volatile (
        "mv a0, %0\n\t"
        "mv a1, %1 \n\t"
        "li a7, 1024\n\t"
        "ecall"
        : "=r"(fd)
        : "r"(path), "r"(mode)
        : "a0", "a1", "a7"
    );
    return fd;
}

void rars_close_file(int fd) {
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a7, 57\n\t"
        "ecall"
        : "=r"(fd)
        : "r"(fd)
        : "a0", "a1", "a7"
    );
}

long rars_lseek(int fd, int whence) {
    long position = -1;
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a1, 0\n\t"
        "mv a2, %1\n\t"
        "li a7, 62\n\t"
        "ecall"
        : "=r"(position)
        : "r"(fd), "r"(whence)
        : "a0", "a1", "a2", "a7"
    );
    return position;
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
    long position = -1;
    __asm__ volatile (
        "mv a0, %0\n\t"
        "mv a1, %1\n\t"
        "mv a2, %2\n\t"
        "li a7, 63\n\t"
        "ecall"
        : "=r"(position)
        : "r"(fd), "r"(buf), "r"(nbytes)
        : "a0", "a1", "a2", "a7"
    );
    return position;
}

void* rars_sbrk(size_t nbytes) {
    void* ptr = NULL;
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a7, 9\n\t"
        "ecall"
        : "=r"(ptr)
        : "r"(nbytes)
        : "a0"
    );
    return ptr;
}

void rars_exit2(int code) {
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a7, 93\n\t"
        "ecall"
        :
        : "r"(code)
        : "a0"
    );
    return;
}
