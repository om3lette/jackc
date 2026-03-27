#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "test_parser_utils.h"
#include "tau.h"

TEST_F_SETUP(parser_fixture) {
    test_parser_fixture_init(tau, "");
}

TEST_F_TEARDOWN(parser_fixture) {
    test_parser_fixture_destroy(tau);
}

TEST_F(parser_fixture, error_report) {
    ast_var_dec* var = parse_var("static TestClass x, y, z;", tau);
    tau->engine.source = tau->lexer->buffer;

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);

    jackc_diagnostic_engine_report(&tau->engine, tau->lexer->line);
}
