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

TEST_F(parser_fixture, param_list_one_arg) {
    ast_var_dec* var = parse_param_list("int x)", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_VAR(var, VAR_ARG, TYPE_INT, "x");
}

TEST_F(parser_fixture, param_list_one_arg_class) {
    ast_var_dec* var = parse_param_list("TestClass x)", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_CLASS_VAR(var, VAR_ARG, "TestClass", "x");
}

TEST_F(parser_fixture, param_list_many_primitives_same_type) {
    ast_var_dec* var = parse_param_list("int x, int y, int z)", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_VAR(var, VAR_ARG, TYPE_INT, "z");
    REQUIRE_VAR(var->next, VAR_ARG, TYPE_INT, "y");
    REQUIRE_VAR(var->next->next, VAR_ARG, TYPE_INT, "x");
}

TEST_F(parser_fixture, param_list_many_primitives_different_types) {
    ast_var_dec* var = parse_param_list("char x, int y, boolean z)", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_VAR(var, VAR_ARG, TYPE_BOOLEAN, "z");
    REQUIRE_VAR(var->next, VAR_ARG, TYPE_INT, "y");
    REQUIRE_VAR(var->next->next, VAR_ARG, TYPE_CHAR, "x");
}

TEST_F(parser_fixture, param_list_many_classes) {
    ast_var_dec* var = parse_param_list("Carrot x, Stepladder y, Cucumber z)", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_CLASS_VAR(var, VAR_ARG, "Cucumber", "z");
    REQUIRE_CLASS_VAR(var->next, VAR_ARG, "Stepladder", "y");
    REQUIRE_CLASS_VAR(var->next->next, VAR_ARG, "Carrot", "x");
}

TEST_F(parser_fixture, param_list_missing_type) {
    ast_var_dec* var = parse_param_list("char x, y, boolean z)", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}

TEST_F(parser_fixture, param_list_missing_rparen) {
    ast_var_dec* var = parse_param_list("char x, int y, boolean z;", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}

TEST_F(parser_fixture, param_list_missing_identifier) {
    ast_var_dec* var = parse_param_list("char , int y)", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}
