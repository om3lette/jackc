#include "compiler/ast/ast.h"
#include "std/jackc_string.h"
#include "test_parser_utils.h"
#include "tau.h"

TEST_F_SETUP(parser_fixture) {
    test_parser_fixture_init(tau, "");
}

TEST_F_TEARDOWN(parser_fixture) {
    test_parser_fixture_destroy(tau);
}

static void require_class(
    const struct parser_fixture* tau,
    const ast_class* class,
    const char* name,
    const uint32_t expected_class_var_decs,
    const uint32_t expected_subroutines
) {
    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE(class);

    CHECK(jackc_streq(&class->name, name));

    CHECK(var_len(class->class_vars) == expected_class_var_decs);
    CHECK(subroutine_len(class->subroutines) == expected_subroutines);
}

typedef struct {
    const char* source;
    const char* name;
    uint32_t expected_class_var_decs;
    uint32_t expected_subroutines;
} class_test_config;

#define REGISTER_TEST(_name, ...)                                         \
    TEST_F(parser_fixture, _name) {                                       \
        const class_test_config cfg = (class_test_config){ __VA_ARGS__ }; \
        const ast_class* class = parse_class(cfg.source, tau);            \
        require_class(                                                    \
            tau, class,                                                   \
            cfg.name,                                                     \
            cfg.expected_class_var_decs,                                  \
            cfg.expected_subroutines                                      \
        );                                                                \
    }

REGISTER_TEST(class_empty,
    .source = "class Main {}",
    .name = "Main",
    .expected_class_var_decs = 0,
    .expected_subroutines = 0
)
REGISTER_TEST(class_with_static_vars,
    .source = "class Main { static int x, y; static char z; }",
    .name = "Main",
    .expected_class_var_decs = 3,
    .expected_subroutines = 0
)
REGISTER_TEST(class_with_static_and_field_vars,
    .source = "class Main { static int x, y; field boolean m; static char z; }",
    .name = "Main",
    .expected_class_var_decs = 4,
    .expected_subroutines = 0
)
REGISTER_TEST(class_with_subroutines,
    .source = "class Main { static int x, y; function void v() {} method int length() {} }",
    .name = "Main",
    .expected_class_var_decs = 2,
    .expected_subroutines = 2
)
