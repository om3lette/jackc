#include "core/jackc_file_utils.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MAX_DIR_STACK 32

typedef struct {
    HANDLE find_handle;
    char* path_string;
    WIN32_FIND_DATAW find_data;
    int has_pending_file;
} DirState;

static DirState dir_stack[MAX_DIR_STACK];
static int stack_top = -1;

static wchar_t* utf8_to_utf16(const char* str) {
    static wchar_t wide_str_buf[MAX_PATH * 2];
    int wide_length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (wide_length == 0) return NULL;
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wide_str_buf, wide_length);
    return wide_str_buf;
}

static char* utf16_to_utf8(const wchar_t* wstr) {
    static char utf8_buf[MAX_PATH * 2];
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (utf8_len == 0) return NULL;
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8_buf, sizeof(utf8_buf), NULL, NULL);
    return utf8_buf;
}

static int has_extension(const char* filename, const char* ext) {
    const char* dot = jackc_strrchr(filename, '.');
    if (!dot || dot == filename) return 0;
    return jackc_strcmp(dot, ext) == 0;
}

static char* join_path(const char* dir, const char* file) {
    size_t len = jackc_strlen(dir) + jackc_strlen(file) + 2;
    char* full_path = jackc_alloc(len);
    snprintf(full_path, len, "%s/%s", dir, file);
    return full_path;
}

jackc_file_return_code jackc_next_source_file(const char* base_path, const char* extension, const char** out_next_file) {
    if (stack_top == -1) {
        size_t base_len = jackc_strlen(base_path);
        char wildcard_path_buf[MAX_PATH];
        if (base_len + 3 > MAX_PATH) return FILE_FAILED_TO_OPEN_DIR;

        snprintf(wildcard_path_buf, sizeof(wildcard_path_buf), "%s/*", base_path);
        const wchar_t* wildcard_path = utf8_to_utf16(wildcard_path_buf);
        if (!wildcard_path) return FILE_FAILED_TO_OPEN_DIR;

        HANDLE find_handle = FindFirstFileW(wildcard_path, &dir_stack[0].find_data);
        if (find_handle == INVALID_HANDLE_VALUE) {
            DWORD err = GetLastError();
            if (err == ERROR_NO_MORE_FILES || err == ERROR_FILE_NOT_FOUND) {
                *out_next_file = NULL;
                return FILE_EXHAUSTED_DIR;
            }
            return FILE_FAILED_TO_OPEN_DIR;
        }

        ++stack_top;
        dir_stack[stack_top].find_handle = find_handle;
        dir_stack[stack_top].path_string = jackc_alloc(jackc_strlen(base_path) + 1);
        jackc_strcpy(dir_stack[stack_top].path_string, base_path);
        dir_stack[stack_top].has_pending_file = 1;
    }

    while (stack_top >= 0) {
        DirState* current = &dir_stack[stack_top];

        if (!current->has_pending_file) {
            if (!FindNextFileW(current->find_handle, &current->find_data)) {
                FindClose(current->find_handle);
                jackc_free(current->path_string);
                stack_top--;
                continue;
            }
            current->has_pending_file = 1;
        }

        current->has_pending_file = 0;

        const char* entry_name = utf16_to_utf8(current->find_data.cFileName);
        if (!entry_name) continue;

        if (jackc_strcmp(entry_name, ".") == 0 || jackc_strcmp(entry_name, "..") == 0) {
            continue;
        }

        char* full_path = join_path(current->path_string, entry_name);
        if (!full_path) continue;

        DWORD attrib = current->find_data.dwFileAttributes;
        if (attrib & FILE_ATTRIBUTE_DIRECTORY) {
            if (stack_top < MAX_DIR_STACK - 1) {
                char sub_wildcard[MAX_PATH];
                snprintf(sub_wildcard, sizeof(sub_wildcard), "%s/*", full_path);
                wchar_t* wildcard = utf8_to_utf16(sub_wildcard);
                if (!wildcard) {
                    jackc_free(full_path);
                    continue;
                }

                HANDLE sub_handle = FindFirstFileW(wildcard, &dir_stack[stack_top + 1].find_data);
                if (sub_handle == INVALID_HANDLE_VALUE) {
                    jackc_free(full_path);
                    continue;
                }

                stack_top++;
                dir_stack[stack_top].find_handle = sub_handle;
                dir_stack[stack_top].path_string = full_path;
                dir_stack[stack_top].has_pending_file = 1;
                continue;
            } else {
                jackc_free(full_path);
                return FILE_MAX_DIR_DEPTH_REACHED;
            }
        } else if (!(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
            if (has_extension(entry_name, extension)) {
                *out_next_file = full_path;
                return FILE_OK;
            }
            jackc_free(full_path);
        } else {
            jackc_free(full_path);
        }
    }

    *out_next_file = NULL;
    return FILE_EXHAUSTED_DIR;
}
