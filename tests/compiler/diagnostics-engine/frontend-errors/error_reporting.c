#include "compiler/frontend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/jackc_file_utils.h"
#include "core/logging/logger.h"
#include "core/localization/locale.h"
#include "std/jackc_syscalls.h"
#include "tau.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "test_path_utils.h"

struct frontend_fixture {
    Allocator allocator;
};

TEST_F_SETUP(frontend_fixture) {
    tau->allocator = arena_allocator_adapter();
}

TEST_F_TEARDOWN(frontend_fixture) {
    arena_allocator_destroy(tau->allocator.context);
}

TEST_F(frontend_fixture, error_reporting) {
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
        char expected_path[PATH_MAX];
        char output_path[PATH_MAX];

        path_join(expected_path, sizeof(expected_path), test_dir, EXPECTED_FILENAME);
        path_join(output_path, sizeof(output_path), test_dir, OUTPUT_FILENAME);

        char* expected = nullptr;
        if (jackc_read_file_content(expected_path, &expected) != FILE_OK) {
            is_success = false;
            LOG_ERROR("Missing %s for test %s\n", EXPECTED_FILENAME, test_dir);
            continue;
        }

        int out = jackc_open(output_path, O_WRONLY | O_CREAT | O_TRUNC);
        if (out < 0) {
            perror("open");
            free(expected);
            continue;
        }

        const char* input_paths[] = { test_dir };
        jackc_frontend_config config = {
            .locale = &jackc_locale_en,
            .diag_engine_output_fd = out,
            .diag_engine_filename_override = true,
            .skip_vm_code_gen = true
        };
        jackc_frontend_compile(input_paths, 1, nullptr, &config, &tau->allocator);
        jackc_close(out);

        char* actual = nullptr;
        jackc_read_file_content(output_path, &actual);

        bool result = actual ? strcmp(actual, expected) == 0 : false;
        ++tests_total;
        tests_passed += result;
        if (!result) {
            LOG_ERROR("Test '%s' result: FAIL\n", test_dir);
            is_success = false;
        }

        free(actual);
        free(expected);
    } while (next_test_case(base_path, test_dir));

    LOG_INFO("%d/%d tests passed\n", tests_passed, tests_total);
    REQUIRE(is_success);
}
