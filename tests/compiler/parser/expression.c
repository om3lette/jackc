#include "compiler/ast/ast.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "jackc_stdlib.h"
#include "test_parser_utils.h"
#include "test_lexer_common.h"
#include "core/allocators/adapters.h"
#include "tau.h"
#include <assert.h>

TEST_F_SETUP(parser_fixture) {
    tau->arena = arena_allocator_adapter();
    tau->lexer = jack_lexer_init("");
    tau->engine = jack_diag_engine_init();
}

TEST_F_TEARDOWN(parser_fixture) {
    arena_allocator_destroy(tau->arena.context);
    jackc_free(tau->lexer);
}

typedef struct {
    const char* input;
    const char* expected;
} expr_test_case;

#define X(input, expected) { input, expected },

#define REGISTER_TEST(name)                                                            \
    TEST_F(parser_fixture, name) {                                                     \
        const uint32_t idx = __COUNTER__;                                              \
        const char* const input = tests[idx].input;                                    \
        const char* const expected = tests[idx].expected;                              \
        test_jack_lexer_new_buffer(tau->lexer, input);                                 \
        jack_parser* parser = jack_parser_init(tau->lexer, &tau->engine, &tau->arena); \
        ast_expr* e = jack_parser_parse_expression(parser, 0);                         \
        REQUIRE_NO_PANIC(parser);                                                      \
        const char* str = ast_expression_to_string(&tau->arena, e);                    \
        REQUIRE_STREQ(str, expected);                                                  \
    }

static const expr_test_case tests[] = {
    // Unary operators
    { "-a", "-(a)" },
    { "~-a", "~(-(a))" },
    { "-a * b", "(-(a) * b)" },

    // Mixed operators, precedence check
    { "a + b * c - d / e", "((a + (b * c)) - (d / e))" },
    { "a * b + c / d", "((a * b) + (c / d))" },

    // Right-associative operators
    { "a = b = c = d", "(a = (b = (c = d)))" },
    // Implicit this
    { "abs(x) + 1", "(this.abs(x) + 1)" },

    // Function calls with arguments
    { "Math.max(a, b + c)", "Math.max(a, (b + c))" },
    { "foo(1 + 2, bar(x))", "this.foo((1 + 2), this.bar(x))" },
    { "f(g(h(x)))", "this.f(this.g(this.h(x)))" },

    // Member access
    { "obj.getter(3)", "obj.getter(3)" },
    { "2 + obj.call(x)", "(2 + obj.call(x))" },

    // Literals
    { "null + 2", "(null + 2)" },
    { "true & false", "(true & false)" },

    // Combinations with parentheses to override precedence
    { "(a + b) * (c - d)", "((a + b) * (c - d))" },
    { "a * (b + c) / d", "((a * (b + c)) / d)" },
};

REGISTER_TEST(unary)
REGISTER_TEST(nested_unary)
REGISTER_TEST(unary_precedence)

REGISTER_TEST(mixed_1)
REGISTER_TEST(mixed_2)

REGISTER_TEST(right_associative)
REGISTER_TEST(implicit_this)

REGISTER_TEST(function_call_with_arguments)
REGISTER_TEST(function_call_with_arguments_implicit_this)
REGISTER_TEST(nested_function_call_with_arguments_implicit_this)

REGISTER_TEST(member_access)
REGISTER_TEST(op_with_member_access)

REGISTER_TEST(null_literal_op) // This is semantically invalid, but the parser does not concern itself with semantic validity
REGISTER_TEST(true_false_literal_op)

REGISTER_TEST(paren_precedence_overwrite_1)
REGISTER_TEST(paren_precedence_overwrite_2)

static_assert(__COUNTER__ == sizeof(tests) / sizeof(tests[0]), "Some tests are not registered");
