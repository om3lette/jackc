#include "test_path_utils.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

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

bool next_test_case(const char* base_path, char* out_dir) {
    static DIR* dir = nullptr;

    if (!dir) {
        dir = opendir(base_path);
        if (!dir) {
            perror("opendir");
            return false;
        }
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char subdir[PATH_MAX];
        path_join(subdir, sizeof(subdir), base_path, entry->d_name);

        struct stat st;
        if (stat(subdir, &st) != 0 || !S_ISDIR(st.st_mode)) {
            continue;
        }

        char test_file[PATH_MAX];
        path_join(test_file, sizeof(test_file), subdir, TEST_FILENAME);

        if (stat(test_file, &st) == 0 && S_ISREG(st.st_mode)) {
            strncpy(out_dir, subdir, PATH_MAX);
            out_dir[PATH_MAX - 1] = '\0';
            return true;
        }
    }

    closedir(dir);
    dir = nullptr;
    return false;
}
