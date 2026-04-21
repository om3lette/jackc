#include "core/jackc_file_utils.h"
#include "std/jackc_stdlib.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_DIR_STACK 32

typedef struct {
    DIR* dir_handle;
    char* path_string;
} DirState;

static DirState dir_stack[MAX_DIR_STACK];
static int stack_top = -1;

static int has_extension(const char* filename, const char* ext) {
    const char* dot = jackc_strrchr(filename, '.');
    if (!dot || dot == filename) return 0;
    return jackc_strcmp(dot, ext) == 0;
}

static char* join_path(const char* dir, const char* file) {
    // +2 for '/' and null terminator
    size_t len = jackc_strlen(dir) + jackc_strlen(file) + 2;
    char* full_path = jackc_alloc(len);
    snprintf(full_path, len, "%s/%s", dir, file);
    return full_path;
}

jackc_file_return_code jackc_next_source_file(const char* base_path, const char* extension, const char** out_next_file) {
    if (stack_top == -1) {
        DIR* d = opendir(base_path);
        if (!d)
            return FILE_FAILED_TO_OPEN_DIR;

        stack_top++;
        dir_stack[stack_top].dir_handle = d;
        dir_stack[stack_top].path_string = strdup(base_path);
    }

    struct dirent* entry;
    while (stack_top >= 0) {
        entry = readdir(dir_stack[stack_top].dir_handle);

        if (entry == NULL) {
            // Close directory, free path string, and pop stack
            closedir(dir_stack[stack_top].dir_handle);
            jackc_free(dir_stack[stack_top].path_string);
            stack_top--;
            continue;
        }

        if (jackc_strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char* full_path = join_path(dir_stack[stack_top].path_string, entry->d_name);
        if (!full_path) continue;

        struct stat s;
        if (stat(full_path, &s) == -1) {
            jackc_free(full_path);
            return FILE_FAILED_STAT;
        }

        if (S_ISDIR(s.st_mode)) {
            if (stack_top < MAX_DIR_STACK - 1) {
                DIR* sub_dir = opendir(full_path);
                if (sub_dir) {
                    stack_top++;
                    dir_stack[stack_top].dir_handle = sub_dir;
                    dir_stack[stack_top].path_string = full_path;
                    continue;
                }
            } else {
                jackc_free(full_path);
                return FILE_MAX_DIR_DEPTH_REACHED;
            }
            jackc_free(full_path);
        } else if (S_ISREG(s.st_mode)) {
            if (has_extension(entry->d_name, extension)) {
                *out_next_file = full_path;
                return FILE_OK;
            }
            // Undesired file type
            jackc_free(full_path);
        } else {
            jackc_free(full_path);
        }
    }

    *out_next_file = nullptr;
    return FILE_EXHAUSTED_DIR;
}
