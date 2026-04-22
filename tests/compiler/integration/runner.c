#include "compiler/frontend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/localization/locale.h"
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
#define REGISTER_TEST(name, _path, _outcome)                                        \
    TEST_F(integration_fixture, name) {                                             \
        const char* full_path = jackc_join_path(                                    \
            tau->test_folder_path, _path, &tau->allocator                           \
        );                                                                          \
        const char* input_paths[] = { full_path };                                  \
        jackc_frontend_config config = {                                            \
            .locale = &jackc_locale_en,                                             \
            .diag_engine_output_fd = -1,                                            \
            .skip_vm_code_gen = true                                                \
        };                                                                          \
        CHECK_EQ(                                                                   \
            jackc_frontend_compile(                                                 \
                input_paths, 1, nullptr, &config, &tau->allocator                   \
            ),                                                                      \
            _outcome                                                                \
        );                                                                          \
    }

REGISTER_TEST(one_file_program, "one-file-program", FRONTEND_OK)
REGISTER_TEST(multi_file_program, "multi-file-program", FRONTEND_OK)
REGISTER_TEST(var_shadowing_class, "var-shadowing-class", FRONTEND_OK)
REGISTER_TEST(method_call_inside_constructor, "method-call-inside-constructor", FRONTEND_OK)
REGISTER_TEST(string_literal_assignment, "string-literal-assignment", FRONTEND_OK)
REGISTER_TEST(string_literal_assignment_array, "string-literal-assignment-array", FRONTEND_OK)
REGISTER_TEST(subroutine_class_can_be_casted_to_primitives, "subroutine-class-can-be-casted-to-primitives", FRONTEND_OK)

REGISTER_TEST(mixing_declarations_and_code_1, "mixing-declarations-and-code-1", FRONTEND_SYNTAX_ERROR)
REGISTER_TEST(mixing_declarations_and_code_2, "mixing-declarations-and-code-2", FRONTEND_SYNTAX_ERROR)

REGISTER_TEST(method_call_with_no_instance, "method-call-with-no-instance", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(var_shadowing_subroutine, "var-shadowing-subroutine", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(missing_subroutine, "missing-subroutine", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(class_filename_mismatch, "class-filename-mismatch", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(string_literal_assignment_to_non_string, "string-literal-assignment-to-non-string", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(subroutine_call_args_number_mismatch, "subroutine-call-args-number-mismatch", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(undefined_var_as_subroutine_arg, "undefined-var-as-subroutine-arg", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(variable_undefined_class_type, "variable-undefined-class-type", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(missing_entrypoint, "missing-entrypoint", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(subroutine_explicitly_returning_void, "subroutine-explicitly-returning-void", FRONTEND_SEMANTICALLY_INVALID)
REGISTER_TEST(subroutine_return_type_mismatch_primitive, "subroutine-return-type-mismatch-primitive", FRONTEND_SEMANTICALLY_INVALID)
