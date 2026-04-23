#include "core/allocators/allocators.h"
#include "core/jackc_file_utils.h"
#include <stdarg.h>

char* file_content = nullptr;

jackc_file_return_code jackc_dir_iterator_create(const char* base_path, Allocator* allocator, jackc_dir_iterator* out) {
    jackc_dir_iterator iter = {
        .allocator = allocator,
        .stack_top = 0
    };

    char* base_path_copy = allocator->alloc(jackc_strlen(base_path) + 1, allocator->context);
    jackc_strcpy(base_path_copy, base_path);
    iter.dir_stack[0].path = jackc_string_from_str(base_path_copy);

    *out = iter;
    return FILE_OK;
}

jackc_file_return_code jackc_dir_iterator_next_file_with_ext(jackc_dir_iterator* iter, const char* extension, const char** next_file) {
    (void)extension;
    jackc_file_return_code file_ret_code;

    // RARS does not support directory traversal so this has to be done manually
    // This function will only parse the file located at base_path
    if (!file_content) {
        if ((file_ret_code = jackc_read_file_content(iter->dir_stack[0].path.data, &file_content, iter->allocator)) != FILE_OK)
            return file_ret_code;
    }
    if (!file_content) return FILE_FAILED_TO_READ;

    const char* const line_start = file_content;
    while (*file_content != '\0' && *file_content != '\n') {
        ++file_content;
    }

    // Reached EOF
    if (*file_content == '\0')
        return FILE_EXHAUSTED_DIR;
    *file_content++ = '\0';

    *next_file = line_start;
    return FILE_OK;
}
