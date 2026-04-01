#include "compiler/ast/ast.h"
#include "compiler/lexer/compiler_lexer.h"
#include "test_parser_utils.h"
#include "tau.h"

TEST_F_SETUP(parser_fixture) {
    test_parser_fixture_init(tau, "");
}

TEST_F_TEARDOWN(parser_fixture) {
    test_parser_fixture_destroy(tau);
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

TEST_F(parser_fixture, var_dec_many_1) {
    ast_var_dec* var = parse_var("var char x, y, z;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_VAR(var, VAR_LOCAL, TYPE_CHAR, "x");
    REQUIRE_VAR(var->next, VAR_LOCAL, TYPE_CHAR, "y");
    REQUIRE_VAR(var->next->next, VAR_LOCAL, TYPE_CHAR, "z");
}

TEST_F(parser_fixture, var_dec_missing_identifier) {
    ast_var_dec* var = parse_var("var boolean ;", tau);

    REQUIRE_NO_PANIC(tau->parser);
    REQUIRE_ERRORS(tau->parser, 1);
    REQUIRE(var == nullptr);
}

TEST_F(parser_fixture, var_dec_invalid_kind) {
    ast_var_dec* var = parse_var("static TestClass x, y, z;", tau);

    REQUIRE_PANIC(tau->parser, 1);
    REQUIRE(var == nullptr);
}
