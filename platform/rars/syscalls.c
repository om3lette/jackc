#include "rars/rars_syscalls.h"
#include "std/jackc_syscalls.h"

int jackc_open(const char *path, int flags) {
    file_mode mode = O_RDONLY;

    if (flags & O_WRONLY) mode = WRITE_CREATE_MODE;
    else if (flags & O_APPEND) mode = WRITE_APPEND_MODE;

    return rars_open_file(path, mode);
}

long jackc_read(int fd, void* buf, size_t n) {
    return rars_read(fd, buf, n);
}

long jackc_write(int fd, const void *buf, size_t n) {
    return rars_write(fd, buf, n);
}

long jackc_lseek(int fd, long offset, int whence) {
    return rars_lseek(fd, offset, whence);
}

int jackc_close(int fd) {
    rars_close_file(fd);
    return 0;
}
