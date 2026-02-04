#include "common/logger.h"
#include "jackc_stdio.h"
#include "jackc_stdlib.h"
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
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0;
    return strcmp(dot, ext) == 0;
}

static char* join_path(const char* dir, const char* file) {
    // +2 for '/' and null terminator
    size_t len = strlen(dir) + strlen(file) + 2;
    char* full_path = jackc_alloc(len);
    snprintf(full_path, len, "%s/%s", dir, file);
    return full_path;
}

const char* jackc_next_source_file(const char* base_path, const char* extension) {
    if (stack_top == -1) {
        DIR* d = opendir(base_path);
        if (!d) {
            LOG_ERROR("Failed to open base directory %s\n", base_path);
            return NULL;
        }

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

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        char* full_path = join_path(dir_stack[stack_top].path_string, entry->d_name);
        if (!full_path) continue;

        struct stat s;
        if (stat(full_path, &s) == -1) {
            LOG_ERROR("Failed to stat %s\n", full_path);
            jackc_free(full_path);
            continue;
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
                LOG_ERROR("Max directory depth reached\n");
            }
            jackc_free(full_path);
        } else if (S_ISREG(s.st_mode)) {
            if (has_extension(entry->d_name, extension)) return full_path;
            // Undesired file type
            jackc_free(full_path);
        } else {
            jackc_free(full_path);
        }
    }

    return NULL;
}

void jackc_putchar(char c) {
    putchar(c);
}

void jackc_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void jackc_vprintf(const char* format, va_list args) {
    vprintf(format, args);
}

void jackc_vfprintf(int fd, const char* format, va_list args) {
    vdprintf(fd, format, args);
}

void jackc_fprintf(int fd, const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vfprintf(fd, format, args);
    va_end(args);
}

int jackc_open(const char* path, int flags) {
    return open(path, flags, 0644);
}

long jackc_read(int fd, void* buf, size_t n) {
    return read(fd, buf, n);
}

long jackc_write(int fd, const void* buf, size_t n) {
    return write(fd, buf, n);
}

long jackc_lseek(int fd, long offset, int whence) {
    return lseek(fd, offset, whence);
}

void jackc_vsprintf(char* buffer, const char* format, va_list args) {
    vsprintf(buffer, format, args);
}

void jackc_sprintf(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    jackc_vsprintf(buffer, format, args);
    va_end(args);
}

int jackc_close(int fd) {
    return close(fd);
}
