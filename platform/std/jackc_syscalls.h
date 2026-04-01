#ifndef JACKC_SYSCALLS_H
#define JACKC_SYSCALLS_H

#include <stddef.h>

#define JACKC_SEEK_SET	0	/* Seek from beginning of file.  */
#define JACKC_SEEK_CUR	1	/* Seek from current position.  */
#define JACKC_SEEK_END	2	/* Seek from end of file.  */

#define O_CREAT 0100
#define O_RDONLY 00
#define O_WRONLY 01
#define O_APPEND 0200
#define O_TRUNC 01000

int jackc_open(const char* path, int flags);

long jackc_read(int fd, void* buf, size_t n);

long jackc_write(int fd, const void* buf, size_t n);

long jackc_lseek(int fd, long offset, int whence);

int jackc_close(int fd);

#endif
