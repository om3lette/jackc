#include <fcntl.h>
#include <unistd.h>

int jackc_open(const char* path, int flags) {
    return open(path, flags, 0644);
}

long jackc_read(int fd, void* buf, size_t n) {
    return read(fd, buf, n);
}

long jackc_write(int fd, const void* buf, size_t n) {
    return write(fd, buf, n);
}

long jackc_lseek(int fd, long offset, int whence) {
    return lseek(fd, offset, whence);
}

int jackc_close(int fd) {
    return close(fd);
}
