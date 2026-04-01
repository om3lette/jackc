#include "std/jackc_string.h"
#include "std/jackc_stdlib.h"
#include <stdarg.h>

char* file_content = NULL;

const char* jackc_next_source_file(const char* base_path, const char* extension) {
    (void)extension;

    // RARS does not support directory traversal so this has to be done manually
    // This function will only parse the file located at base_path
    if (!file_content) file_content = jackc_read_file_content(base_path);
    if (!file_content) return NULL;

    const char* const line_start = file_content;
    while (*file_content != '\0' && *file_content != '\n') {
        ++file_content;
    }

    // Reached EOF
    if (*file_content == '\0') return NULL;
    *file_content++ = '\0';

    return line_start;
}
