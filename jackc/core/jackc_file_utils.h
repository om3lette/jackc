#ifndef JACKC_FILE_UTILS_H
#define JACKC_FILE_UTILS_H

#include "core/allocators/allocators.h"
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

/**
 * Opens a file by given path, reads file content, returns a pointer to a buffer.
 *
 * Buffer is allocated inside of a function.
 *
 * @param path Path to file.
 */
[[ nodiscard ]] jackc_file_return_code jackc_read_file_content(const char* file_path, char** out_file_content, Allocator* allocator);

[[ nodiscard ]] const char* jackc_find_filename(const char* path);

[[ nodiscard ]] jackc_string jackc_find_filename_no_ext(const char* path);

[[ nodiscard ]] bool jackc_has_extension(const char* filename, const char* extension);

[[ nodiscard ]] const char* jackc_join_path(const char* dir, const char* file, Allocator* allocator);

// TODO: Add windows support
#ifdef __rars__
    // Placeholder. RARS is unable to open directories
    #define DIRECTORY char
#else
    #include <dirent.h>
    #define DIRECTORY DIR*
#endif

#define MAX_DIR_DEPTH 32

typedef struct {
    DIRECTORY dir;
    jackc_string path;
} jackc_dir_with_path;

typedef struct {
    uint32_t stack_top;
    Allocator* allocator;
    jackc_dir_with_path dir_stack[MAX_DIR_DEPTH];
} jackc_dir_iterator;

[[ nodiscard ]] jackc_file_return_code jackc_dir_iterator_create(const char* base_path, Allocator* allocator, jackc_dir_iterator* out);

[[ nodiscard ]] jackc_file_return_code jackc_dir_iterator_next_file_with_ext(jackc_dir_iterator* iter, const char* extension, const char** next_file);

#endif
