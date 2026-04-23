#include <fcntl.h>
#include <unistd.h>
#include "std/jackc_syscalls.h"

int jackc_open(const char* path, FLAGS flags) {
    FLAGS extra_flags = flags & O_WRONLY ? O_TRUNC : 0;
    return open(path, flags | O_CREAT | extra_flags, 0644);
}

long jackc_read(FD fd, void* buf, size_t n) {
    return read(fd, buf, n);
}

long jackc_write(FD fd, const void* buf, size_t n) {
    return write(fd, buf, n);
}

long jackc_lseek(FD fd, long offset, FLAGS whence) {
    return lseek(fd, offset, whence);
}

int jackc_close(FD fd) {
    return close(fd);
}
