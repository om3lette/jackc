#include "test_path_utils.h"
#include "core/jackc_file_utils.h"
#include "core/localization/locale.h"
#include "std/jackc_limits.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_stdio.h"
#include "std/jackc_string.h"

bool test_streq_ignore_carriage_return(const char* a, const char* b) {
    while (*a && * b && *a++ && *b++) {
        if (*a == '\r') ++a;
        if (*b == '\r') ++b;
        if (*a != *b)
            return false;
    }
    return *a == *b;
}


void path_dirname(char* out, size_t size, const char* path) {
    const char* last_slash = jackc_strrchr(path, '/');
#ifdef _WIN32
    const char* last_backslash = jackc_strrchr(path, '\\');
    if (last_backslash && (!last_slash || last_backslash > last_slash))
        last_slash = last_backslash;
#endif
    if (!last_slash) {
        out[0] = '\0';
        return;
    }

    size_t len = (size_t)(last_slash - path);
    if (len >= size) len = size - 1;

    jackc_memcpy(out, path, len);
    out[len] = '\0';
}

void get_test_root(const char* runner_path, char* out) {
    char dir[PATH_MAX * 2];

    path_dirname(dir, sizeof(dir), runner_path);
    jackc_sprintf(out, "%s%ctests", dir, DELIMITER);
}

bool next_test_case(jackc_dir_iterator* iter, const char** out_dir) {
    char current_dir[PATH_MAX] = "";
    const char* current_file_directory = "";

    while (jackc_strcmp(current_dir, current_file_directory) == 0) {
        jackc_file_return_code file_ret_code = FILE_OK;

        if ((file_ret_code = jackc_dir_iterator_next_file_with_ext(iter, ".jack", &current_file_directory)) != FILE_OK) {
            if (file_ret_code != FILE_EXHAUSTED_DIR)
                jackc_report_file_error(&jackc_locale_en, file_ret_code, iter->dir_stack[0].path.data);
            return false;
        }

        path_dirname((char*)current_file_directory, jackc_strlen(current_file_directory), current_file_directory);
    }

    jackc_strcpy(current_dir, current_file_directory);
    *out_dir = current_file_directory;
    return true;
}
