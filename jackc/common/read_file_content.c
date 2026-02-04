// TODO: Move to common, use jackc_* instead of syscalls and libc functions
#include "common/logger.h"
#include "jackc_stdio.h"
#include "jackc_stdlib.h"

char* jackc_read_file_content(const char* file_path) {
    int fd = jackc_open(file_path, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }
    LOG_DEBUG("Opened file: %s\n", file_path);

    long file_size = jackc_lseek(fd, 0L, JACKC_SEEK_END);
    if (file_size < 0) {
        LOG_ERROR("Failed to reach end of file.\n");
        jackc_close(fd);
        return NULL;
    }
    if (jackc_lseek(fd, 0L, JACKC_SEEK_SET) < 0) {
        LOG_ERROR("Failed to rewind.\n");
        jackc_close(fd);
        return NULL;
    }
    LOG_DEBUG("Calculated size: %ld bytes.\n", file_size);

    char* content_buffer = jackc_alloc((size_t)(file_size + 1));

    long bytes_read = jackc_read(fd, content_buffer, (size_t)file_size);
    content_buffer[bytes_read] = '\0';
    if(jackc_close(fd) < 0) {
        LOG_ERROR("Failed to close file descriptor.\n");
        return NULL;
    }

    if (bytes_read != file_size) {
        LOG_ERROR("Expected %ld bytes, but read %ld bytes.\n", file_size, bytes_read);
        return NULL;
    }

    LOG_DEBUG("Saved the content to a buffer.\n");
    return content_buffer;
}
