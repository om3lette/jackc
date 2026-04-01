#include "test_path_utils.h"
#include <stdio.h>
#include <string.h>

void path_dirname(char* out, size_t size, const char* path) {
    const char* last_slash = strrchr(path, '/');

    // Does not support Windows paths with backslashes
    if (!last_slash) {
        out[0] = '\0';
        return;
    }

    size_t len = (size_t)(last_slash - path);
    if (len >= size) len = size - 1;

    memcpy(out, path, len);
    out[len] = '\0';
}

void get_test_root(const char* runner_path, char* out, size_t size) {
    char dir[PATH_MAX];

    path_dirname(dir, sizeof(dir), runner_path);
    snprintf(out, size, "%s/tests", dir);
}

void path_join(char* out, size_t size, const char* a, const char* b) {
    if (!a) a = "";
    if (!b) b = "";

    int written = snprintf(out, size, "%s/%s", a, b);
    if (written < 0 || (size_t)written >= size) {
        out[size - 1] = '\0';
    }
}
