#include "compiler/symtable/compiler_symtable.h"
#include <tau.h>
#include "utils.h"

TEST_F_SETUP(symtab_fixture) {
    test_symtab_common_init(tau);
}

TEST_F_TEARDOWN(symtab_fixture) {
    test_symtab_common_teardown(tau);
}

TEST_F(symtab_fixture, push_one) {
    sym_table* top_symtab = sym_table_push(tau->symtab, &tau->allocator);
    REQUIRE(top_symtab);
    REQUIRE(top_symtab != tau->symtab);
    sym_table_free(&top_symtab);
}

TEST_F(symtab_fixture, push_many) {
    sym_table* top_symtab = tau->symtab;
    for (uint32_t i = 0; i < 10; ++i) {
        top_symtab = sym_table_push(top_symtab, &tau->allocator);
        REQUIRE(top_symtab);
    }
    sym_table_free(&top_symtab);
}

TEST_F(symtab_fixture, pop) {
    sym_table* top_symtab = sym_table_push(tau->symtab, &tau->allocator);
    REQUIRE_EQ((void*)sym_table_pop(top_symtab), (void*)tau->symtab);
    sym_table_free(&tau->symtab);
}

TEST_F(symtab_fixture, pop_many) {
    const uint32_t ITERATIONS = 10;
    sym_table* symtabs[ITERATIONS + 1];
    symtabs[0] = tau->symtab;

    sym_table* top_symtab = tau->symtab;
    for (uint32_t i = 1; i < ITERATIONS + 1; ++i) {
        top_symtab = sym_table_push(top_symtab, &tau->allocator);
        REQUIRE(top_symtab && top_symtab != symtabs[i - 1]);
        symtabs[i] = top_symtab;
    }

    for (uint32_t i = 0; i < ITERATIONS; ++i) {
        top_symtab = sym_table_pop(top_symtab);
        // Ensure that the popped symtab matches the expected symtab (order is preserved)
        REQUIRE_EQ((void*)top_symtab, (void*)symtabs[ITERATIONS - i - 1]);
    }

    sym_table_free(&tau->symtab);
}
