#ifndef JACKC_RARS_SYSCALLS_H
#define JACKC_RARS_SYSCALLS_H

#include <stddef.h>

int rars_open_file(const char* path);

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


#endif
