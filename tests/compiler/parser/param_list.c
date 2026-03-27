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
