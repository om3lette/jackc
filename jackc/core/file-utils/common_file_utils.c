#include "core/allocators/allocators.h"
#include "std/jackc_stdio.h"
#include "std/jackc_string.h"
#include "std/jackc_syscalls.h"
#include "core/jackc_file_utils.h"

jackc_file_return_code jackc_read_file_content(const char* file_path, char** out_file_content, Allocator* allocator) {
    FD fd = jackc_open(file_path, O_RDONLY);
    if (fd < 0)
        return FILE_FAILED_TO_OPEN;

    long file_size = jackc_lseek(fd, 0L, JACKC_SEEK_END);
    if (file_size < 0) {
        jackc_close(fd);
        return FILE_FAILED_LSEEK;
    }
    if (jackc_lseek(fd, 0L, JACKC_SEEK_SET) < 0) {
        jackc_close(fd);
        return FILE_FAILED_REWIND;
    }

    char* content_buffer = allocator->alloc((size_t)(file_size + 1), allocator->context);

    long bytes_read = jackc_read(fd, content_buffer, (size_t)file_size);
    content_buffer[bytes_read] = '\0';
    if(jackc_close(fd) < 0)
        return FILE_FAILED_TO_CLOSE;

    if (bytes_read != file_size)
        return FILE_FAILED_TO_READ;

    *out_file_content = content_buffer;
    return FILE_OK;
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

bool jackc_has_extension(const char* filename, const char* extension) {
    const char* dot = jackc_strrchr(filename, '.');
    if (!dot || dot == filename)
        return false;
    return jackc_strcmp(dot, extension) == 0;
}

const char* jackc_join_path(const char* dir, const char* file, Allocator* allocator) {
    #ifndef _WIN32
    const char delimiter[] = "/";
    #else
    const char delimiter[] = "\\";
    #endif

    // +1 for '\0'
    size_t len = jackc_strlen(dir) + jackc_strlen(file) + jackc_strlen(delimiter) + 1;
    char* full_path = allocator->alloc(len, allocator->context);
    jackc_sprintf(full_path, "%s%s%s", dir, delimiter, file);
    return full_path;
}
