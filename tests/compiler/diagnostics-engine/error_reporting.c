#include "jackc_stdlib.h"
#include "test_parser_utils.h"
#include "tau.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define PATH_MAX 4096
#define TEST_FILENAME "source.jack"
#define EXPECTED_FILENAME "expected.txt"
#define OUTPUT_FILENAME "output.txt"

static void path_dirname(char* out, size_t size, const char* path) {
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

static void get_test_root(char* out, size_t size) {
    char dir[PATH_MAX];

    path_dirname(dir, sizeof(dir), __FILE__);
    snprintf(out, size, "%s/tests", dir);
}

static void path_join(char* out, size_t size, const char* a, const char* b) {
    snprintf(out, size, "%s/%s", a, b);
}

#include <sys/stat.h>

bool next_test_case(const char* base_path, char* out_dir) {
    static DIR* dir = nullptr;

    if (base_path) {
        if (dir) {
            closedir(dir);
        }

        dir = opendir(base_path);
        if (!dir) {
            perror("opendir");
            return false;
        }
    }

    if (!dir) return false;

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

        // check if source.jack exists
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

TEST_F_SETUP(parser_fixture) {
    test_parser_fixture_init(tau, "");
}

TEST_F_TEARDOWN(parser_fixture) {
    test_parser_fixture_destroy(tau);
}

TEST_F(parser_fixture, error_reporting) {
    (void)tau;

    char base_path[PATH_MAX];
    get_test_root(base_path, sizeof(base_path));

    char test_dir[PATH_MAX];

    if (!next_test_case(base_path, test_dir)) {
        return;
    }

    bool is_success = true;

    do {
        char source_path[PATH_MAX];
        char expected_path[PATH_MAX];
        char output_path[PATH_MAX];

        path_join(source_path, sizeof(source_path), test_dir, TEST_FILENAME);
        path_join(expected_path, sizeof(expected_path), test_dir, EXPECTED_FILENAME);
        path_join(output_path, sizeof(output_path), test_dir, OUTPUT_FILENAME);

        char* source = jackc_read_file_content(source_path);
        char* expected = jackc_read_file_content(expected_path);

        FILE* out = fopen(output_path, "w");
        if (!out) {
            perror("fopen");
            continue;
        }

        parse_class(source, tau);
        tau->engine.output_fd = fileno(out);
        tau->engine.filename = TEST_FILENAME;
        tau->engine.source = tau->lexer->buffer;
        jackc_diagnostic_engine_report(&tau->engine, tau->lexer->line);
        fflush(out);
        fclose(out);

        char* actual = jackc_read_file_content(output_path);
        bool result = strcmp(actual, expected) == 0;
        if (!result) {
            char* test_name = strrchr(source_path, '/');
            jackc_printf("Test '%s' failed\n", test_name ? test_name + 1 : source_path);
        }
        is_success &= result;

        free(actual);
        free(source);
        free(expected);

    } while (next_test_case(nullptr, test_dir));

    REQUIRE(is_success);
}
