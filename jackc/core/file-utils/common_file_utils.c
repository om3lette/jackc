#include "core/logging/logger.h"
#include "std/jackc_stdio.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"
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

const char* jackc_find_filename(const char* path) {
    const char* last_sep = path;
    for (const char* p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') {
            last_sep = p + 1;
        }
    }
    return last_sep;
}

jackc_string jackc_find_filename_no_ext(const char* path) {
    const char* filename_start = jackc_find_filename(path);
    const char* filename_end = jackc_strrchr(filename_start, '.');
    if (!filename_start || !filename_end) {
        return jackc_string_empty();
    }
    return jackc_string_create(filename_start, (size_t)(filename_end - filename_start));
}
