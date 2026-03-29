#include "compiler/diagnostics-engine/engine.h"
#include "core/logging/logger.h"
#include "jackc_stdlib.h"
#include "test_parser_utils.h"
#include "test_lexer_common.h"
#include "tau.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "test_path_utils.h"

#define TEST_FILENAME "source.jack"
#define EXPECTED_FILENAME "expected.txt"
#define OUTPUT_FILENAME "output.txt"

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
    get_test_root(__FILE__, base_path, sizeof(base_path));

    char test_dir[PATH_MAX];

    if (!next_test_case(base_path, test_dir)) {
        REQUIRE(false, "No test cases found");
        return;
    }

    bool is_success = true;
    uint32_t tests_total = 0;
    uint32_t tests_passed = 0;

    do {
        char source_path[PATH_MAX];
        char expected_path[PATH_MAX];
        char output_path[PATH_MAX];

        path_join(source_path, sizeof(source_path), test_dir, TEST_FILENAME);
        path_join(expected_path, sizeof(expected_path), test_dir, EXPECTED_FILENAME);
        path_join(output_path, sizeof(output_path), test_dir, OUTPUT_FILENAME);

        char* source = jackc_read_file_content(source_path);
        if (!source) {
            is_success = false;
            LOG_ERROR("Missing %s for test %s\n", TEST_FILENAME, test_dir);
            continue;
        }
        char* expected = jackc_read_file_content(expected_path);
        if (!expected) {
            is_success = false;
            LOG_ERROR("Missing %s for test %s\n", EXPECTED_FILENAME, test_dir);
            free(source);
            continue;
        }

        FILE* out = fopen(output_path, "w");
        if (!out) {
            perror("fopen");
            continue;
        }

        test_jack_lexer_new_buffer(&tau->lexer, source);
        jackc_diag_engine_reset(&tau->engine, tau->lexer.buffer, TEST_FILENAME, fileno(out));

        parse_class(source, tau);
        jackc_diagnostic_engine_report(&tau->engine, tau->lexer.line);
        fflush(out);
        fclose(out);

        char* actual = jackc_read_file_content(output_path);
        bool result = strcmp(actual, expected) == 0;
        ++tests_total;
        tests_passed += result;
        if (!result) {
            LOG_ERROR("Test '%s' result: FAIL\n", source_path);
            is_success = false;
        }

        free(actual);
        free(source);
        free(expected);
    } while (next_test_case(base_path, test_dir));

    LOG_INFO("%d/%d tests passed\n", tests_passed, tests_total);
    REQUIRE(is_success);
}
