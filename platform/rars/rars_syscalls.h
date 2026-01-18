#ifndef JACKC_RARS_SYSCALLS_H
#define JACKC_RARS_SYSCALLS_H

#include <stddef.h>

typedef enum {
    READ_ONLY_MODE = 0,
    WRITE_CREATE_MODE = 1,
    WRITE_APPEND_MODE = 9
} file_mode;

int rars_open_file(const char* path, file_mode mode);

void rars_close_file(int fd);

long rars_lseek(int fd, int whence);

/**
 * RARS Read syscall wrapper.
 *
 * @param fd File descriptor.
 * @param buf Buffer to write to.
 * @param nbytes Max number of bytes to read.
 *
 * @returns Number of bytes read.
 */
long rars_read(int fd, void* buf, size_t nbytes);

void* rars_sbrk(size_t nbytes);

void rars_exit2(int code);

#endif
