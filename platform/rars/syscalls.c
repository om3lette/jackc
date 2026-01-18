#include "rars_syscalls.h"

int rars_open_file(const char* path) {
    int fd = -1;
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a1, 1 \n\t"
        "li a7, 1024\n\t"
        "ecall"
        : "=r"(fd)
        : "r"(path)
        : "a0", "a1", "a7"
    );
    return fd;
}

long rars_lseek(int fd, int whence) {
    long position = -1;
    __asm__ volatile (
        "mv a0, %0\n\t"
        "li a1, 0\n\t"
        "li a2, %1\n\t"
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
