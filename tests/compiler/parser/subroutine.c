#include "compiler/ast/ast.h"
#include "std/jackc_string.h"
#include "test_ast_types.h"
#include "test_parser_utils.h"
#include "tau.h"

#undef VOID

TEST_F_SETUP(parser_fixture) {
    test_parser_fixture_init(tau, "");
}

TEST_F_TEARDOWN(parser_fixture) {
    test_parser_fixture_destroy(tau);
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

typedef struct {
    const char* source;
    ast_sub_kind kind;
    ast_type return_type;
    const char* name;
    uint32_t expected_arg_count;
    uint32_t expected_vars_count;
    uint32_t expected_statements_count;
} subroutine_test_config;

#define REGISTER_TEST(_name, ...)                                                   \
    TEST_F(parser_fixture, _name) {                                                 \
        const subroutine_test_config cfg = (subroutine_test_config){ __VA_ARGS__ }; \
        const ast_subroutine* sub = parse_subroutine(cfg.source, tau);              \
        require_subroutine(                                                         \
            tau, sub,                                                               \
            cfg.kind,                                                               \
            &cfg.return_type,                                                       \
            cfg.name,                                                               \
            cfg.expected_arg_count,                                                 \
            cfg.expected_vars_count,                                                \
            cfg.expected_statements_count                                           \
        );                                                                          \
    }

REGISTER_TEST(subroutine_test_function, 
    .source = "function void main() {}",
    .kind = SUB_FUNCTION,
    .return_type = T_VOID,
    .name = "main",
    .expected_arg_count = 0,
    .expected_vars_count = 0,
    .expected_statements_count = 0
)
REGISTER_TEST(subroutine_test_native_function, 
    .source = "native function void main() {}",
    .kind = SUB_FUNCTION,
    .return_type = T_VOID,
    .name = "main",
    .expected_arg_count = 0,
    .expected_vars_count = 0,
    .expected_statements_count = 0
)
REGISTER_TEST(subroutine_test_function_many_var_decs, 
    .source = "function void main() { var int x, y; var char a, b; }",
    .kind = SUB_FUNCTION,
    .return_type = T_VOID,
    .name = "main",
    .expected_arg_count = 0,
    .expected_vars_count = 4,
    .expected_statements_count = 0
)
REGISTER_TEST(subroutine_test_method, 
    .source = "method char main() {}",
    .kind = SUB_METHOD,
    .return_type = T_CHAR,
    .name = "main",
    .expected_arg_count = 0,
    .expected_vars_count = 0,
    .expected_statements_count = 0
)
REGISTER_TEST(subroutine_test_constructor, 
    .source = "constructor Number init() {}",
    .kind = SUB_CONSTRUCTOR,
    .return_type = T_CLASS("Number"),
    .name = "init",
    .expected_arg_count = 0,
    .expected_vars_count = 0,
    .expected_statements_count = 0
)
REGISTER_TEST(subroutine_with_arguments, 
    .source = "constructor Number init(int x, int y, boolean abs) {}",
    .kind = SUB_CONSTRUCTOR,
    .return_type = T_CLASS("Number"),
    .name = "init",
    .expected_arg_count = 3,
    .expected_vars_count = 0,
    .expected_statements_count = 0
)
REGISTER_TEST(subroutine_with_arguments_and_vars, 
    .source = "constructor Number init(int x, int y, boolean abs) { var int xy, abs_xy; }",
    .kind = SUB_CONSTRUCTOR,
    .return_type = T_CLASS("Number"),
    .name = "init",
    .expected_arg_count = 3,
    .expected_vars_count = 2,
    .expected_statements_count = 0
)
REGISTER_TEST(subroutine_with_arguments_and_vars_and_statements, 
    .source = "function int calculate(int x, int y, boolean abs) { var int xy, abs_xy; let xy = x * y; if (abs) { let abs_xy = Math.abs(xy); } else { let abs_xy = xy; } return abs_xy; }",
    .kind = SUB_FUNCTION,
    .return_type = T_INT,
    .name = "calculate",
    .expected_arg_count = 3,
    .expected_vars_count = 2,
    .expected_statements_count = 3
)
