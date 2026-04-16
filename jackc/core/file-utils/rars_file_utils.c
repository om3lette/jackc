#include "core/jackc_file_utils.h"
#include <stdarg.h>

char* file_content = nullptr;

jackc_file_return_code jackc_next_source_file(const char* base_path, const char* extension, const char** out_next_file) {
    (void)extension;
    jackc_file_return_code file_ret_code;

    // RARS does not support directory traversal so this has to be done manually
    // This function will only parse the file located at base_path
    if (!file_content) {
        if ((file_ret_code = jackc_read_file_content(base_path, &file_content)) != FILE_OK)
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

    *out_next_file = line_start;
    return FILE_OK;
}
