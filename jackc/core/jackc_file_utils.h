#ifndef JACKC_FILE_UTILS_H
#define JACKC_FILE_UTILS_H

#include "std/jackc_string.h"

typedef enum {
    FILE_OK,
    FILE_EXHAUSTED_DIR,
    FILE_FAILED_TO_OPEN,
    FILE_FAILED_TO_READ,
    FILE_FAILED_TO_CLOSE,
    FILE_FAILED_LSEEK,
    FILE_FAILED_REWIND,
    FILE_FAILED_STAT,
    FILE_FAILED_TO_OPEN_DIR,
    FILE_MAX_DIR_DEPTH_REACHED,
    FILE_FAILED_TO_EXTRACT_NAME
} jackc_file_return_code;

jackc_file_return_code jackc_next_source_file(const char* base_path, const char* extension, const char** out_next_file);

/**
 * Opens a file by given path, reads file content, returns a pointer to a buffer.
 *
 * Buffer is allocated inside of a function.
 *
 * @param path Path to file.
 */
jackc_file_return_code jackc_read_file_content(const char* file_path, char** out_file_content);

const char* jackc_find_filename(const char* path);

jackc_string jackc_find_filename_no_ext(const char* path);

#endif
