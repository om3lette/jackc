#include "compiler/ast/traversals/ast_traversal_utils.h"
#include "tau.h"
#include "test_ast_types.h"

typedef struct {
    ast_type type1;
    ast_type type2;
    bool expected;
} type_compatibility_test;

#define TEST_RUNNER(_tests) \
    for (size_t i = 0; i < sizeof(_tests) / sizeof(_tests[0]); i++) { \
        type_compatibility_test test = _tests[i]; \
        CHECK_EQ(are_types_compatible(&test.type1, &test.type2), test.expected); \
    }

TEST(ast, type_compatibily_primitives) {
    type_compatibility_test tests[] = {
        { T_INT, T_INT, true }, { T_INT, T_CHAR, true }, { T_INT, T_BOOLEAN, true },
        { T_CHAR, T_CHAR, true }, { T_INT, T_BOOLEAN, true },
        { T_BOOLEAN, T_BOOLEAN, true },
    };
    TEST_RUNNER(tests)
}

TEST(ast, type_compatibily_classes) {
    type_compatibility_test tests[] = {
        { T_INT, T_CLASS("Test"), true }, { T_CHAR, T_CLASS("Test"), true },
        { T_BOOLEAN, T_CLASS("Test"), true }, { T_CLASS("Test"), T_CLASS("Test"), true },
        { T_CLASS("Test"), T_CLASS("Different"), false },
    };
    TEST_RUNNER(tests)
}

TEST(ast, type_compatibily_void) {
    type_compatibility_test tests[] = {
        { T_INT, T_VOID, false }, { T_CHAR, T_VOID, false },
        { T_BOOLEAN, T_VOID, false }, { T_VOID, T_VOID, true },
        { T_CLASS("Test"), T_VOID, false }
    };
    TEST_RUNNER(tests)
}
