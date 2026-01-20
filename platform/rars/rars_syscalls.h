#ifndef JACKC_RARS_SYSCALLS_H
#define JACKC_RARS_SYSCALLS_H

#include <stddef.h>


#define RARS_SEEK_SET	0	/* Seek from beginning of file.  */
#define RARS_SEEK_CUR	1	/* Seek from current position.  */
#define RARS_SEEK_END	2	/* Seek from end of file.  */

typedef enum {
    READ_ONLY_MODE = 0,
    WRITE_CREATE_MODE = 1,
    WRITE_APPEND_MODE = 9
} file_mode;

/**
 * RARS Open syscall wrapper.
 *
 * @param path Path to file.
 * @param mode Mode to open file in.
 *
 * @returns File descriptor.
 */
int rars_open_file(const char* path, file_mode mode);

/**
 * RARS Close syscall wrapper.
 *
 * @param fd File descriptor.
 */
void rars_close_file(int fd);

/**
 * RARS Lseek syscall wrapper.
 *
 * @param fd File descriptor.
 * @param offset Offset to seek to.
 * @param whence Whence to seek from.
 *
 * @returns New offset.
 */
long rars_lseek(int fd, long offset, int whence);

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

/**
 * RARS sbrk syscall wrapper.
 *
 * @param nbytes Number of bytes to allocate.
 *
 * @returns Pointer to the allocated memory.
 */
void* rars_sbrk(size_t nbytes);

/**
 * RARS Write syscall wrapper.
 *
 * @param fd File descriptor.
 * @param buf Buffer to read from.
 * @param nbytes Max number of bytes to write.
 *
 * @returns Number of bytes written.
 */
long rars_write(int fd, const void* buf, size_t nbytes);

/**
 * RARS Exit syscall wrapper.
 *
 * @param code Exit code.
 */
void rars_exit2(int code);

/**
 * RARS Print integer syscall wrapper.
 *
 * @param n Integer to print.
 */
void rars_print_int(int n);

/**
 * RARS Print unsigned integer syscall wrapper.
 *
 * @param n Unsigned integer to print.
 */
void rars_print_uint(unsigned int n);

/**
 * RARS Print float syscall wrapper.
 *
 * @param f Float to print.
 */
void rars_print_float(float f);

/**
 * RARS Print string syscall wrapper.
 *
 * @param str String to print.
 */
void rars_print_string(const char* str);

/**
 * RARS Print character syscall wrapper.
 *
 * @param c Character to print.
 */
void rars_print_char(char c);

#endif
