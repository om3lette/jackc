#include "compiler/symtable/compiler_symtable.h"
#include "compiler/symtable/symtable_token.h"
#include "utils.h"
#include <tau.h>

TEST_F_SETUP(symtab_fixture) {
    test_symtab_common_init(tau);
}

TEST_F_TEARDOWN(symtab_fixture) {
    test_symtab_common_teardown(tau);
}

TEST_F(symtab_fixture, insert) {
    sym_table_token token = create_token(JACK_INT, VAR_ARG, "x", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(tau->symtab, &token));
}

TEST_F(symtab_fixture, insert_assigns_token_idx) {
    sym_table_token token = create_token(JACK_INT, VAR_ARG, "x", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(tau->symtab, &token));
    REQUIRE_EQ(token.var.idx, 0);

    token = create_token(JACK_INT, VAR_ARG, "xy", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(tau->symtab, &token));
    REQUIRE_EQ(token.var.idx, 1);
}

TEST_F(symtab_fixture, insert_rejects_duplicate) {
    sym_table_token token = create_token(JACK_INT, VAR_ARG, "x", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(tau->symtab, &token));
    REQUIRE_EQ(sym_table_insert(tau->symtab, &token), SYMTAB_ALREADY_EXISTS);
}
