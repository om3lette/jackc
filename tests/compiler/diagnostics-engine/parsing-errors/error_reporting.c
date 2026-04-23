#include "compiler/diagnostics-engine/engine.h"
#include "core/jackc_file_utils.h"
#include "core/localization/locale.h"
#include "core/logging/logger.h"
#include "std/jackc_limits.h"
#include "std/jackc_syscalls.h"
#include "test_parser_utils.h"
#include "test_lexer_common.h"
#include "tau.h"
#include <sys/stat.h>
#include "test_path_utils.h"

TEST_F_SETUP(parser_fixture) {
    test_parser_fixture_init(tau, "");
}

TEST_F_TEARDOWN(parser_fixture) {
    test_parser_fixture_destroy(tau);
}

TEST_F(parser_fixture, error_reporting) {

    char base_path[PATH_MAX];
    get_test_root(__FILE__, base_path);

    const char* test_dir = nullptr;

    bool is_success = true;
    uint32_t tests_total = 0;
    uint32_t tests_passed = 0;

    jackc_dir_iterator iter;

    REQUIRE_EQ(jackc_dir_iterator_create(base_path, &tau->arena, &iter), FILE_OK);
    while (next_test_case(&iter, &test_dir)) {
        const char* source_path = jackc_join_path(test_dir, TEST_FILENAME, &tau->arena);
        const char* expected_path = jackc_join_path(test_dir, EXPECTED_FILENAME, &tau->arena);
        const char* output_path = jackc_join_path(test_dir, OUTPUT_FILENAME, &tau->arena);

        char* source = nullptr;
        if (jackc_read_file_content(source_path, &source, &tau->arena) != FILE_OK) {
            is_success = false;
            LOG_ERROR("Missing %s for test %s\n", TEST_FILENAME, test_dir);
            continue;
        }
        char* expected = nullptr;

        if (jackc_read_file_content(expected_path, &expected, &tau->arena) != FILE_OK) {
            is_success = false;
            LOG_ERROR("Missing %s for test %s\n", EXPECTED_FILENAME, test_dir);
            continue;
        }

        FD out = jackc_open(output_path, O_WRONLY);
        if (out < 0) {
            is_success = false;
            continue;
        }

        test_jack_lexer_new_buffer(&tau->lexer, source);
        jackc_diag_engine_reset(&tau->engine, tau->lexer.buffer, TEST_FILENAME, out);

        parse_class(source, tau);
        jackc_diagnostic_engine_report(&tau->engine, tau->lexer.line);
        jackc_close(out);

        char* actual = nullptr;
        jackc_file_return_code file_ret_code;
        if ((file_ret_code = jackc_read_file_content(output_path, &actual, &tau->arena) != FILE_OK)) {
            jackc_report_file_error(&jackc_locale_en, file_ret_code, output_path);
            return;
        }

        bool result = actual ? test_streq_ignore_carriage_return(actual, expected) : false;
        ++tests_total;
        tests_passed += result;
        if (!result) {
            LOG_ERROR("Test '%s' result: FAIL\n", source_path);
            is_success = false;
        }
    }

    LOG_INFO("%d/%d tests passed\n", tests_passed, tests_total);
    REQUIRE(tests_passed > 0, "No tests were found");
    REQUIRE(is_success);
}
