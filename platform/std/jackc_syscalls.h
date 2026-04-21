#ifndef JACKC_SYSCALLS_H
#define JACKC_SYSCALLS_H

#include <stddef.h>

#define JACKC_SEEK_SET	0	/* Seek from beginning of file.  */
#define JACKC_SEEK_CUR	1	/* Seek from current position.  */
#define JACKC_SEEK_END	2	/* Seek from end of file.  */

#ifdef _WIN32
#   define _CRT_SECURE_NO_WARNINGS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <stdint.h>
#   define O_RDONLY GENERIC_READ
#   define O_WRONLY GENERIC_WRITE
#   define FD intptr_t
#   define FLAGS DWORD
#else
#   define O_RDONLY 00
#   define O_WRONLY 01
#   define FD int
#   define FLAGS int
#endif

FD jackc_open(const char* path, FLAGS flags);

long jackc_read(FD fd, void* buf, size_t n);

long jackc_write(FD fd, const void* buf, size_t n);

long jackc_lseek(FD fd, long offset, FLAGS whence);

int jackc_close(FD fd);

#endif
