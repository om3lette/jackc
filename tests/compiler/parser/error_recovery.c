#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/adapters.h"
#include "core/allocators/allocators.h"
#include "jackc_stdlib.h"
#include "test_parser_utils.h"
#include "tau.h"

TEST_F_SETUP(parser_fixture) {
    tau->arena = arena_allocator_adapter();
    tau->lexer = jack_lexer_init("");
    tau->engine = jack_diag_engine_init();
    tau->parser = nullptr;
}

TEST_F_TEARDOWN(parser_fixture) {
    arena_allocator_destroy(tau->arena.context);
    jackc_free(tau->lexer);
}

TEST_F(parser_fixture, error_recovery_param_list_missing_type) {
    ast_var_dec* var = parse_param_list("char x, y, boolean z)", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK(var_len(var) == 2);
}

TEST_F(parser_fixture, error_recovery_param_list_missing_rparen) {
    ast_var_dec* var = parse_param_list("char x, int y, boolean z;", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK(var_len(var) == 3);
}

TEST_F(parser_fixture, error_recovery_param_list_missing_identifier) {
    ast_var_dec* var = parse_param_list("char , int y)", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK(var_len(var) == 1);
}

TEST_F(parser_fixture, error_recovery_class_var_dec_invalid_kind) {
    ast_class* cls = parse_class("class t { static int n; var TestClass x, y, z; }", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK_EQ(var_len(cls->class_vars), 1);

    cls = parse_class("class t { var int n; static TestClass x, y, z; }", tau);
    REQUIRE_NO_PANIC(tau->parser);
    CHECK_EQ(var_len(cls->class_vars), 3);
}

TEST_F(parser_fixture, error_recovery_class_var_dec_missing_colon) {
    ast_class* cls = parse_class("class t { static boolean x y z; field int n; }", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK_EQ(var_len(cls->class_vars), 2);
}

TEST_F(parser_fixture, error_recovery_class_invalid_subroutine_kind) {
    ast_class* cls = parse_class("class t { var func() {} method int compute() {} }", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK_EQ(subroutine_len(cls->subroutines), 1);
}

TEST_F(parser_fixture, error_recovery_var_dec_missing_type) {
    // x will actually get interpreted as a class name
    // Error recovery will try to recover a ',' panic and get the following result:
    // var x, y, z -> var x y, z where x is treated as a Class name
    ast_var_dec* var = parse_var("var x, y, z;", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK_EQ(var_len(var), 2);
}
