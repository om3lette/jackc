#include "compiler/ast/ast.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "test_parser_utils.h"
#include "test_lexer_common.h"
#include "tau.h"

TEST_F_SETUP(parser_fixture) {
    test_parser_fixture_init(tau, "");
}

TEST_F_TEARDOWN(parser_fixture) {
    test_parser_fixture_destroy(tau);
}

typedef struct {
    const char* input;
    const char* expected;
} expr_test_config;

#define REGISTER_TEST(name, ...)                                                       \
    TEST_F(parser_fixture, name) {                                                     \
        const expr_test_config cfg = (expr_test_config){ __VA_ARGS__ };                \
        test_jack_lexer_new_buffer(&tau->lexer, cfg.input);                            \
        jack_parser parser = jack_parser_init(&tau->lexer, &tau->engine, &tau->arena); \
        ast_expr* e = jack_parser_parse_expression(&parser, 0);                        \
        REQUIRE_NO_PANIC(parser);                                                      \
        const char* str = ast_expression_to_string(&tau->arena, e);                    \
        REQUIRE_STREQ(str, cfg.expected);                                              \
    }

REGISTER_TEST(unary, 
    .input = "-a", 
    .expected = "-(a)"
)
REGISTER_TEST(nested_unary, 
    .input = "~-a", 
    .expected = "~(-(a))"
)
REGISTER_TEST(unary_precedence, 
    .input = "-a * b", 
    .expected = "(-(a) * b)"
)

REGISTER_TEST(mixed_1, 
    .input = "a + b * c - d / e", 
    .expected = "((a + (b * c)) - (d / e))"
)
REGISTER_TEST(mixed_2, 
    .input = "a * b + c / d", 
    .expected = "((a * b) + (c / d))"
)

REGISTER_TEST(right_associative, 
    .input = "a = b = c = d", 
    .expected = "(a = (b = (c = d)))"
)
REGISTER_TEST(implicit_this, 
    .input = "abs(x) + 1", 
    .expected = "(this.abs(x) + 1)"
)

REGISTER_TEST(function_call_with_arguments, 
    .input = "Math.max(a, b + c)", 
    .expected = "Math.max(a, (b + c))"
)
REGISTER_TEST(function_call_with_arguments_implicit_this, 
    .input = "foo(1 + 2, bar(x))", 
    .expected = "this.foo((1 + 2), this.bar(x))"
)
REGISTER_TEST(nested_function_call_with_arguments_implicit_this, 
    .input = "f(g(h(x)))", 
    .expected = "this.f(this.g(this.h(x)))"
)

REGISTER_TEST(member_access, 
    .input = "obj.getter(3)", 
    .expected = "obj.getter(3)"
)
REGISTER_TEST(op_with_member_access, 
    .input = "2 + obj.call(x)", 
    .expected = "(2 + obj.call(x))"
)

REGISTER_TEST(null_literal_op, 
    .input = "null + 2", 
    .expected = "(null + 2)"
)
REGISTER_TEST(true_false_literal_op, 
    .input = "true & false", 
    .expected = "(true & false)"
)

REGISTER_TEST(paren_precedence_overwrite_1, 
    .input = "(a + b) * (c - d)", 
    .expected = "((a + b) * (c - d))"
)
REGISTER_TEST(paren_precedence_overwrite_2, 
    .input = "a * (b + c) / d", 
    .expected = "((a * (b + c)) / d)"
)
