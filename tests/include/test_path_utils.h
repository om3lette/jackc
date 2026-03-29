#ifndef TEST_PATH_UTILS_H
#define TEST_PATH_UTILS_H

#include <stddef.h>

#define PATH_MAX 4096

void path_dirname(char* out, size_t size, const char* path);

void get_test_root(const char* runner_path, char* out, size_t size);

void path_join(char* out, size_t size, const char* a, const char* b);

#endif
