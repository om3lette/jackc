#ifndef JACKC_STDIO_H
#define JACKC_STDIO_H

#include <stddef.h>
#include <stdarg.h>

#define JACKC_SEEK_SET	0	/* Seek from beginning of file.  */
#define JACKC_SEEK_CUR	1	/* Seek from current position.  */
#define JACKC_SEEK_END	2	/* Seek from end of file.  */

#define O_CREAT 0100
#define O_RDONLY 00
#define O_WRONLY 01
#define O_APPEND 0200
#define O_TRUNC 01000

#ifdef __rars__
    #define EXPECTED_ARGUMENTS 2
    #define FIRST_ARG_IDX 0
#else
    #define EXPECTED_ARGUMENTS 3
    #define FIRST_ARG_IDX 1
#endif

const char* jackc_next_source_file(const char* base_path, const char* extension);

void jackc_vfprintf(int fd, const char* format, va_list args);

void jackc_fprintf(int fd, const char* format, ...);

void jackc_vsprintf(char* buffer, const char* format, va_list args);

void jackc_sprintf(char* buffer, const char* format, ...);

int jackc_open(const char* path, int flags);

long jackc_read(int fd, void* buf, size_t n);

long jackc_lseek(int fd, long offset, int whence);

long jackc_write(int fd, const void* buf, size_t n);

int jackc_close(int fd);

#endif
