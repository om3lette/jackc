#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/adapters.h"
#include "core/allocators/allocators.h"
#include "jackc_stdlib.h"
#include "jackc_string.h"
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
} class_test;

#define REGISTER_TEST(_name)                                     \
    TEST_F(parser_fixture, _name) {                              \
        const uint32_t index = __COUNTER__;                      \
        const class_test* test = &tests[index];                  \
        const ast_class* class = parse_class(test->source, tau); \
        require_class(                                           \
            tau, class,                                          \
            test->name,                                          \
            test->expected_class_var_decs,                       \
            test->expected_subroutines                           \
        );                                                       \
    }

static const class_test tests[] = {
    {
        .source = "class Main {}",
        .name = "Main",
        .expected_class_var_decs = 0,
        .expected_subroutines = 0,
    },
    {
        .source = "class Main { static int x, y; static char z; }",
        .name = "Main",
        .expected_class_var_decs = 3,
        .expected_subroutines = 0,
    },
    {
        .source = "class Main { static int x, y; field boolean m; static char z; }",
        .name = "Main",
        .expected_class_var_decs = 4,
        .expected_subroutines = 0,
    },
    {
        .source = "class Main { static int x, y; function void v() {} method int length() {} }",
        .name = "Main",
        .expected_class_var_decs = 2,
        .expected_subroutines = 2,
    }
};

REGISTER_TEST(class_empty)
REGISTER_TEST(class_with_static_vars)
REGISTER_TEST(class_with_static_and_field_vars)
REGISTER_TEST(class_with_subroutines)

static_assert(__COUNTER__ == sizeof(tests) / sizeof(tests[0]), "Some tests are not registered");
