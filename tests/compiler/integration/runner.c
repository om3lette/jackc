#include "compiler/frontend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "tau.h"
#include "test_path_utils.h"

struct integration_fixture {
    char* test_folder_path;
    Allocator allocator;
};

TEST_F_SETUP(integration_fixture) {
    tau->allocator = arena_allocator_adapter();
    tau->test_folder_path = tau->allocator.alloc(PATH_MAX, tau->allocator.context);
    get_test_root(__FILE__, tau->test_folder_path, PATH_MAX);
}

TEST_F_TEARDOWN(integration_fixture) {
    arena_allocator_destroy(tau->allocator.context);
}

/**
 * Registers a test case for the integration fixture.
 *
 * @param name The name of the test case.
 * @param _path The path to the test folder relative to the `tests` folder.
 * @param _outcome The expected outcome of the test case. (jackc_frontend_return_code)
 */
#define REGISTER_TEST(name, _path, _outcome)                                    \
    TEST_F(integration_fixture, name) {                                         \
        char full_path[PATH_MAX];                                               \
        path_join(full_path, sizeof(full_path), tau->test_folder_path, _path);  \
        CHECK_EQ(jackc_frontend_compile(full_path, &tau->allocator), _outcome); \
    }

REGISTER_TEST(one_file_program, "one-file-program", FRONTEND_OK)
REGISTER_TEST(multi_file_program, "multi-file-program", FRONTEND_OK)
REGISTER_TEST(var_shadowing_class, "var-shadowing-class", FRONTEND_OK)

REGISTER_TEST(method_call_with_no_instance, "method-call-with-no-instance", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(var_shadowing_subroutine, "var-shadowing-subroutine", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(missing_subroutine, "missing-subroutine", FRONTEND_SEMANTICALLY_INVALID)
