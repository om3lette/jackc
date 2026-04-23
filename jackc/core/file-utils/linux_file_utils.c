#include "core/allocators/allocators.h"
#include "core/jackc_file_utils.h"
#include "std/jackc_string.h"
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

jackc_file_return_code jackc_dir_iterator_create(const char* base_path, Allocator* allocator, jackc_dir_iterator* out) {
    DIR* d = opendir(base_path);
    if (!d)
        return FILE_FAILED_TO_OPEN_DIR;

    jackc_dir_iterator iter = {
        .allocator = allocator,
        .stack_top = 0
    };
    iter.dir_stack[0].dir = d;

    char* base_path_copy = allocator->alloc(jackc_strlen(base_path) + 1, allocator->context);
    jackc_strcpy(base_path_copy, base_path);
    iter.dir_stack[0].path = jackc_string_from_str(base_path_copy);

    *out = iter;
    return FILE_OK;
}

jackc_file_return_code jackc_dir_iterator_next_file_with_ext(jackc_dir_iterator* iter, const char* extension, const char** next_file) {
    Allocator* allocator = iter->allocator;
    struct dirent* entry;

    while (true) {
        jackc_dir_with_path top_dir = iter->dir_stack[iter->stack_top];

        entry = readdir(top_dir.dir);

        if (!entry) {
            // Close directory, free path string, and pop stack
            closedir(top_dir.dir);
            allocator->free((void*)top_dir.path.data, top_dir.path.length, allocator->context);
            if (iter->stack_top == 0)
                break;
            --iter->stack_top;
            continue;
        }

        if (jackc_strcmp(entry->d_name, ".") == 0 || jackc_strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        jackc_string full_path = jackc_string_from_str(jackc_join_path(top_dir.path.data, entry->d_name, allocator));
        if (!full_path.data) continue;

        struct stat s;
        if (stat(full_path.data, &s) == -1) {
            allocator->free((void*)full_path.data, full_path.length + 1, allocator->context);
            return FILE_FAILED_STAT;
        }

        if (S_ISDIR(s.st_mode)) {
            if (iter->stack_top < MAX_DIR_DEPTH - 1) {
                DIR* sub_dir = opendir(full_path.data);
                if (sub_dir) {
                    ++iter->stack_top;
                    iter->dir_stack[iter->stack_top].dir = sub_dir;
                    iter->dir_stack[iter->stack_top].path = full_path;
                    continue;
                }
            } else {
                allocator->free((void*)full_path.data, full_path.length + 1, allocator->context);
                return FILE_MAX_DIR_DEPTH_REACHED;
            }
            allocator->free((void*)full_path.data, full_path.length + 1, allocator->context);
        } else if (S_ISREG(s.st_mode)) {
            if (jackc_has_extension(entry->d_name, extension)) {
                *next_file = full_path.data;
                return FILE_OK;
            }
            // Undesired file type
            allocator->free((void*)full_path.data, full_path.length + 1, allocator->context);
        } else {
            allocator->free((void*)full_path.data, full_path.length + 1, allocator->context);
        }
    }

    *next_file = nullptr;
    return FILE_EXHAUSTED_DIR;
}
