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

static ast_var_dec* parse_var(const char* src, struct parser_fixture* tau) {
    test_jack_lexer_new_buffer(tau->lexer, src);
    jack_parser* parser = jack_parser_init(tau->lexer, &tau->engine, &tau->arena);
    tau->parser = parser;
    return jack_parser_parse_var_dec(parser);
}

TEST_F(parser_fixture, var_dec_one) {
    ast_var_dec* var = parse_var("var int x;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_VAR(var, VAR_LOCAL, TYPE_INT, "x");
}

TEST_F(parser_fixture, var_dec_one_class) {
    ast_var_dec* var = parse_var("var TestClass x;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_CLASS_VAR(var, VAR_LOCAL, "TestClass", "x");
}

TEST_F(parser_fixture, var_dec_many) {
    ast_var_dec* var = parse_var("var char x, y, z;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_VAR(var, VAR_LOCAL, TYPE_CHAR, "z");
    REQUIRE_VAR(var->next, VAR_LOCAL, TYPE_CHAR, "y");
    REQUIRE_VAR(var->next->next, VAR_LOCAL, TYPE_CHAR, "x");
}

TEST_F(parser_fixture, var_dec_missing_var) {
    ast_var_dec* var = parse_var("char x, y, z;", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}

TEST_F(parser_fixture, var_dec_missing_type) {
    // x will actually get interpreted as a class name
    // The panic will occur when the parser tries to read the identifier name but ',' is encountered
    ast_var_dec* var = parse_var("var x, y, z;", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}

TEST_F(parser_fixture, var_dec_missing_identifier) {
    ast_var_dec* var = parse_var("var boolean ;", tau);

    REQUIRE_NO_PANIC(tau->parser);
    REQUIRE_ERRORS(tau->parser, 1);
    REQUIRE(var == nullptr);
}

TEST_F(parser_fixture, var_dec_missing_colon) {
    ast_var_dec* var = parse_var("var boolean x y z;", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}

TEST_F(parser_fixture, var_dec_invalid_kind) {
    ast_var_dec* var = parse_var("static TestClass x, y, z;", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}
