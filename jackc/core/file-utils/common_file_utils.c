#include "core/logging/logger.h"
#include "std/jackc_stdio.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_syscalls.h"
#include "core/jackc_file_utils.h"

char* jackc_read_file_content(const char* file_path) {
    int fd = jackc_open(file_path, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }

    long file_size = jackc_lseek(fd, 0L, JACKC_SEEK_END);
    if (file_size < 0) {
        // LOG_ERROR("Failed to reach end of file.\n");
        jackc_close(fd);
        return NULL;
    }
    if (jackc_lseek(fd, 0L, JACKC_SEEK_SET) < 0) {
        LOG_ERROR("Failed to rewind.\n");
        jackc_close(fd);
        return NULL;
    }

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

    return content_buffer;
}
