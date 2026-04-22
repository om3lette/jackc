#ifndef TEST_PATH_UTILS_H
#define TEST_PATH_UTILS_H

#include "core/jackc_file_utils.h"
#include <stddef.h>

#define PATH_MAX 4096

#define TEST_FILENAME "Main.jack"
#define EXPECTED_FILENAME "expected.txt"
#define OUTPUT_FILENAME "output.txt"

void path_dirname(char* out, size_t size, const char* path);

void get_test_root(const char* runner_path, char* out);

[[ nodiscard ]] bool next_test_case(jackc_dir_iterator* iter, const char** out_dir);

#endif
