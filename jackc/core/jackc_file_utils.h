#ifndef JACKC_FILE_UTILS_H
#define JACKC_FILE_UTILS_H

#include "std/jackc_string.h"

const char* jackc_next_source_file(const char* base_path, const char* extension);

/**
 * Opens a file by given path, reads file content, returns a pointer to a buffer.
 *
 * Buffer is allocated inside of a function.
 *
 * @param path Path to file.
 */
char* jackc_read_file_content(const char* file_path);

const char* jackc_find_filename(const char* path);

jackc_string jackc_find_filename_no_ext(const char* path);

#endif
