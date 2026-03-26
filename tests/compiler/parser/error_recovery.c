#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "core/allocators/adapters.h"
#include "core/allocators/allocators.h"
#include "jackc_stdlib.h"
#include "test_lexer_common.h"
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

static ast_var_dec* parse_param_list(const char* src, struct parser_fixture* tau) {
    test_jack_lexer_new_buffer(tau->lexer, src);
    jack_parser* parser = jack_parser_init(tau->lexer, &tau->engine, &tau->arena);
    tau->parser = parser;
    return jack_parser_parse_param_list(parser);
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

static ast_class* parse_class(const char* src, struct parser_fixture* tau) {
    test_jack_lexer_new_buffer(tau->lexer, src);
    jack_parser* parser = jack_parser_init(tau->lexer, &tau->engine, &tau->arena);
    tau->parser = parser;
    return jack_parser_parse_class(parser);
}

TEST_F(parser_fixture, error_recovery_class_var_dec_invalid_kind) {
    ast_class* cls = parse_class("class t { static int n; var TestClass x, y, z; }", tau);

    REQUIRE_NO_PANIC(tau->parser);
    CHECK_EQ(var_len(cls->class_vars), 1);
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
