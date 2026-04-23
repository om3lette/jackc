#include "compiler/frontend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/jackc_file_utils.h"
#include "core/logging/logger.h"
#include "core/localization/locale.h"
#include "std/jackc_limits.h"
#include "std/jackc_syscalls.h"
#include "tau.h"
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
    get_test_root(__FILE__, base_path);

    const char* test_dir = nullptr;

    bool is_success = true;
    uint32_t tests_total = 0;
    uint32_t tests_passed = 0;

    jackc_dir_iterator iter;
    REQUIRE_EQ(jackc_dir_iterator_create(base_path, &tau->allocator, &iter), FILE_OK);
    while (next_test_case(&iter, &test_dir)) {
        const char* expected_path = jackc_join_path(test_dir, EXPECTED_FILENAME, &tau->allocator);
        const char* output_path = jackc_join_path(test_dir, OUTPUT_FILENAME, &tau->allocator);

        char* expected = nullptr;
        if (jackc_read_file_content(expected_path, &expected, &tau->allocator) != FILE_OK) {
            is_success = false;
            LOG_ERROR("Missing %s for test %s\n", EXPECTED_FILENAME, test_dir);
            continue;
        }

        FD out = jackc_open(output_path, O_WRONLY);
        if (out < 0) {
            is_success = false;
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
        jackc_file_return_code file_ret_code;
        if ((file_ret_code = jackc_read_file_content(output_path, &actual, &tau->allocator) != FILE_OK)) {
            jackc_report_file_error(&jackc_locale_en, file_ret_code, output_path);
            return;
        }

        bool result = actual ? test_streq_ignore_carriage_return(actual, expected) : false;
        ++tests_total;
        tests_passed += result;
        if (!result) {
            LOG_ERROR("Test '%s' result: FAIL\n", test_dir);
            is_success = false;
        }
    }

    LOG_INFO("%d/%d tests passed\n", tests_passed, tests_total);
    REQUIRE(tests_passed > 0, "No tests were found");
    REQUIRE(is_success);
}
