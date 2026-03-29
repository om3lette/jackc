#include "compiler/ast/ast.h"
#include "compiler/function-registry/function_registry.h"
#include "jackc_string.h"
#include "utils.h"
#include "tau.h"

TEST_F_SETUP(function_registry_fixture) {
    test_function_registry_common_setup(tau);
}

TEST_F_TEARDOWN(function_registry_fixture) {
    test_function_registry_common_teardown(tau);
}

static ast_var_dec create_argument(char* name, ast_type_kind type_kind, ast_var_dec* next) {
    return (ast_var_dec){
        .kind = VAR_ARG,
        .name = jackc_string_from_str(name),
        .type = {
            .kind = type_kind,
            .class_name = jackc_string_from_str(""),
        },
        .next = next,
    };
}

static ast_subroutine create_subroutine(char* name, ast_sub_kind kind, ast_var_dec* params, ast_subroutine* next) {
    return (ast_subroutine){
        .kind = kind,
        .return_type = {
            .kind = TYPE_BOOLEAN,
        },
        .name = jackc_string_from_str(name),
        .params = params,
        .locals = nullptr,
        .body = nullptr,
        .next = next,
    };
}

static ast_class create_class(char* name, ast_subroutine* subroutines) {
    return (ast_class){
        .name = jackc_string_from_str(name),
        .class_vars = nullptr,
        .subroutines = subroutines,
    };
}

TEST_F(function_registry_fixture, insert_class_registers_all_subroutines) {
    ast_var_dec sub_2_param_2 = create_argument("arg4", TYPE_BOOLEAN, nullptr);
    ast_var_dec sub_2_param_1 = create_argument("arg3", TYPE_CLASS, &sub_2_param_2);
    ast_subroutine sub_2 = create_subroutine("method-the-second", SUB_METHOD, &sub_2_param_1, nullptr);

    ast_var_dec sub_1_param_2 = create_argument("arg2", TYPE_INT, nullptr);
    ast_var_dec sub_1_param_1 = create_argument("arg1", TYPE_CHAR, &sub_1_param_2);
    ast_subroutine sub_1 = create_subroutine("method-the-first", SUB_METHOD, &sub_1_param_1, &sub_2);


    ast_class class = create_class("TestClass", &sub_1);
    function_registry_insert(tau->registry, &class);

    REQUIRE(function_registry_contains_class(tau->registry, &class.name, nullptr));

    // Subroutine 1
    function_signature sub_1_signature;
    CHECK(function_registry_contains(tau->registry, &class.name, &sub_1.name, &sub_1_signature));
    CHECK(jackc_streq(&sub_1_signature.arguments->name, "arg1"));
    CHECK_EQ(sub_1_signature.arguments->type.kind, TYPE_CHAR);

    CHECK(jackc_streq(&sub_1_signature.arguments->next->name, "arg2"));
    CHECK_EQ(sub_1_signature.arguments->next->type.kind, TYPE_INT);

    CHECK_EQ(sub_1_signature.return_type->kind, TYPE_BOOLEAN);

    // Subroutine 2
    function_signature sub_2_signature;
    CHECK(function_registry_contains(tau->registry, &class.name, &sub_2.name, &sub_2_signature));
    CHECK(jackc_streq(&sub_2_signature.arguments->name, "arg3"));
    CHECK_EQ(sub_2_signature.arguments->type.kind, TYPE_CLASS);

    CHECK(jackc_streq(&sub_2_signature.arguments->next->name, "arg4"));
    CHECK_EQ(sub_2_signature.arguments->next->type.kind, TYPE_BOOLEAN);

    CHECK_EQ(sub_2_signature.return_type->kind, TYPE_BOOLEAN);
}
