#include "core/allocators/allocators.h"
#include "core/jackc_file_utils.h"
#include "std/jackc_string.h"
#include <minwinbase.h>
#include <stdio.h>

static wchar_t* utf8_to_utf16(const char* str) {
    static wchar_t wide_str_buf[MAX_PATH * 2];
    int wide_length = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (wide_length == 0)
        return nullptr;
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wide_str_buf, wide_length);
    return wide_str_buf;
}

static char* utf16_to_utf8(const wchar_t* wstr) {
    static char utf8_buf[MAX_PATH * 2];
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (utf8_len == 0)
        return nullptr;
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8_buf, sizeof(utf8_buf), nullptr, nullptr);
    return utf8_buf;
}

jackc_file_return_code jackc_dir_iterator_create(const char* base_path, Allocator* allocator, jackc_dir_iterator* out) {
    jackc_dir_iterator iter = {
        .allocator = allocator,
        .stack_top = 0
    };

    char* base_path_copy = allocator->alloc(jackc_strlen(base_path) + 1, allocator->context);
    jackc_strcpy(base_path_copy, base_path);

    size_t base_len = jackc_strlen(base_path);
    char wildcard_path_buf[MAX_PATH];
    if (base_len + 3 > MAX_PATH)
        return FILE_FAILED_TO_OPEN_DIR;

    snprintf(wildcard_path_buf, sizeof(wildcard_path_buf), "%s\\*", base_path);
    const wchar_t* wildcard_path = utf8_to_utf16(wildcard_path_buf);
    if (!wildcard_path)
        return FILE_FAILED_TO_OPEN_DIR;

    WIN32_FIND_DATAW data;
    HANDLE find_handle = FindFirstFileW(wildcard_path, &data);
    if (find_handle == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        if (err == ERROR_NO_MORE_FILES || err == ERROR_FILE_NOT_FOUND)
            return FILE_EXHAUSTED_DIR;
        return FILE_FAILED_TO_OPEN_DIR;
    }

    iter.dir_stack[0].dir = find_handle;
    iter.dir_stack[0].path = jackc_string_from_str(base_path_copy);

    *out = iter;
    return FILE_OK;
}

jackc_file_return_code jackc_dir_iterator_next_file_with_ext(jackc_dir_iterator* iter, const char* extension, const char** next_file) {
    Allocator* allocator = iter->allocator;
    while (true) {
        jackc_dir_with_path* current = &iter->dir_stack[iter->stack_top];

        WIN32_FIND_DATAW find_data;
        if (!FindNextFileW(current->dir, &find_data)) {
            FindClose(current->dir);
            allocator->free((void*)(current->path).data, (current->path).length + 1, allocator->context);
            if (iter->stack_top == 0)
                break;
            --iter->stack_top;
            continue;
        }

        const char* entry_name = utf16_to_utf8(find_data.cFileName);
        if (!entry_name) continue;

        if (jackc_strcmp(entry_name, ".") == 0 || jackc_strcmp(entry_name, "..") == 0) {
            continue;
        }

        jackc_string full_path = jackc_string_from_str(jackc_join_path(current->path.data, entry_name, allocator));
        if (!full_path.data) continue;

        DWORD attrib = find_data.dwFileAttributes;
        if (attrib & FILE_ATTRIBUTE_DIRECTORY) {
            if (iter->stack_top < MAX_DIR_DEPTH - 1) {
                char sub_wildcard[MAX_PATH];
                snprintf(sub_wildcard, sizeof(sub_wildcard), "%s\\\\*", full_path.data);
                wchar_t* wildcard = utf8_to_utf16(sub_wildcard);
                if (!wildcard) {
                    allocator->free((void*)(current->path).data, (current->path).length + 1, allocator->context);
                    continue;
                }

                HANDLE sub_handle = FindFirstFileW(wildcard, &find_data);
                if (sub_handle == INVALID_HANDLE_VALUE) {
                    allocator->free((void*)(current->path).data, (current->path).length + 1, allocator->context);
                    continue;
                }

                ++iter->stack_top;
                iter->dir_stack[iter->stack_top].dir = sub_handle;
                iter->dir_stack[iter->stack_top].path = full_path;
                continue;
            } else {
                allocator->free((void*)(current->path).data, (current->path).length + 1, allocator->context);
                return FILE_MAX_DIR_DEPTH_REACHED;
            }
        } else if (!(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
            if (jackc_has_extension(entry_name, extension)) {
                *next_file = full_path.data;
                return FILE_OK;
            }
            allocator->free((void*)(current->path).data, (current->path).length + 1, allocator->context);
        } else {
            allocator->free((void*)(current->path).data, (current->path).length + 1, allocator->context);
        }
    }

    *next_file = nullptr;
    return FILE_EXHAUSTED_DIR;
}
