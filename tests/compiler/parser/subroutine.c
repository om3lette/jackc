#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "core/allocators/adapters.h"
#include "core/allocators/allocators.h"
#include "jackc_stdlib.h"
#include "jackc_string.h"
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

static ast_subroutine* parse_subroutine(const char* src, struct parser_fixture* tau) {
    test_jack_lexer_new_buffer(tau->lexer, src);
    jack_parser* parser = jack_parser_init(tau->lexer, &tau->engine, &tau->arena);
    tau->parser = parser;
    return jack_parser_parse_subroutine(parser);
}

static void require_subroutine(
    const struct parser_fixture* tau,
    const ast_subroutine* sub,
    const ast_sub_kind kind,
    const ast_type* return_type,
    const char* name,
    const uint32_t expected_arg_count,
    const uint32_t expected_vars_count,
    const uint32_t expected_statements_count
) {
    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE(sub);
    CHECK(sub->kind == kind);

    CHECK(sub->return_type.kind == return_type->kind);
    if (sub->return_type.kind == TYPE_CLASS) {
        CHECK(jackc_string_cmp(&sub->return_type.class_name, &return_type->class_name) == 0);
    }

    CHECK(jackc_streq(&sub->name, name));

    uint32_t arg_count = 0;
    ast_var_dec* cur_param = sub->params;
    while (cur_param) {
        ++arg_count;
        cur_param = cur_param->next;
    }
    CHECK(arg_count == expected_arg_count);

    uint32_t vars_count = 0;
    ast_var_dec* cur_local = sub->locals;
    while (cur_local) {
        ++vars_count;
        cur_local = cur_local->next;
    }
    CHECK(vars_count == expected_vars_count);

    uint32_t statements_count = 0;
    ast_stmt* cur_stmt = sub->body;
    while (cur_stmt) {
        ++statements_count;
        cur_stmt = cur_stmt->next;
    }
    CHECK(statements_count == expected_statements_count);
}

#define EMPTY_CLASS_NAME { .data = nullptr, .length = 0 }
#define INT { .kind = TYPE_INT, .class_name = EMPTY_CLASS_NAME }
#define VOID { .kind = TYPE_VOID, .class_name = EMPTY_CLASS_NAME }
#define CHAR { .kind = TYPE_CHAR, .class_name = EMPTY_CLASS_NAME }
#define BOOLEAN { .kind = TYPE_BOOLEAN, .class_name = EMPTY_CLASS_NAME }
#define CLASS(name) { .kind = TYPE_CLASS, .class_name = { .data = name, .length = sizeof(name) - 1 } }

typedef struct {
    char* source;
    ast_sub_kind kind;
    ast_type return_type;
    char* name;
    uint32_t expected_arg_count;
    uint32_t expected_vars_count;
    uint32_t expected_statements_count;
} subroutine_test;

#define REGISTER_TEST(_name)                                             \
    TEST_F(parser_fixture, _name) {                                      \
        const uint32_t index = __COUNTER__;                              \
        const subroutine_test* test = &tests[index];                     \
        const ast_subroutine* sub = parse_subroutine(test->source, tau); \
        require_subroutine(                                              \
            tau, sub,                                                         \
            test->kind,                                                  \
            &test->return_type,                                          \
            test->name,                                                  \
            test->expected_arg_count,                                    \
            test->expected_vars_count,                                   \
            test->expected_statements_count                              \
        );                                                               \
    }

static const subroutine_test tests[] = {
    {
        .source = "function void main() {}",
        .kind = SUB_FUNCTION,
        .return_type = VOID,
        .name = "main",
        .expected_arg_count = 0,
        .expected_vars_count = 0,
        .expected_statements_count = 0,
    },
    {
        .source = "method char main() {}",
        .kind = SUB_METHOD,
        .return_type = CHAR,
        .name = "main",
        .expected_arg_count = 0,
        .expected_vars_count = 0,
        .expected_statements_count = 0,
    },
    {
        .source = "constructor Number init() {}",
        .kind = SUB_CONSTRUCTOR,
        .return_type = CLASS("Number"),
        .name = "init",
        .expected_arg_count = 0,
        .expected_vars_count = 0,
        .expected_statements_count = 0,
    },
    {
        .source = "constructor Number init(int x, int y, boolean abs) {}",
        .kind = SUB_CONSTRUCTOR,
        .return_type = CLASS("Number"),
        .name = "init",
        .expected_arg_count = 3,
        .expected_vars_count = 0,
        .expected_statements_count = 0,
    },
    {
        .source = "constructor Number init(int x, int y, boolean abs) { var int xy, abs_xy; }",
        .kind = SUB_CONSTRUCTOR,
        .return_type = CLASS("Number"),
        .name = "init",
        .expected_arg_count = 3,
        .expected_vars_count = 2,
        .expected_statements_count = 0,
    },
    {
        .source =
        "function int calculate(int x, int y, boolean abs) {"
            "var int xy, abs_xy;"
            "let xy = x * y;"
            "if (abs) {"
                "let abs_xy = Math.abs(xy);"
            "} else {"
                "let abs_xy = xy;"
            "}"
            "return abs_xy;"
        "}",
        .kind = SUB_FUNCTION,
        .return_type = INT,
        .name = "calculate",
        .expected_arg_count = 3,
        .expected_vars_count = 2,
        .expected_statements_count = 3,
    },
};

REGISTER_TEST(subroutine_test_function)
REGISTER_TEST(subroutine_test_method)
REGISTER_TEST(subroutine_test_constructor)
REGISTER_TEST(subroutine_with_arguments)
REGISTER_TEST(subroutine_with_arguments_and_vars)
REGISTER_TEST(subroutine_with_arguments_and_vars_and_statements)
static_assert(__COUNTER__ == sizeof(tests) / sizeof(tests[0]), "Some tests are not registered");
