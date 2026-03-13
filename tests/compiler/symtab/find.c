#include "compiler/symtable/compiler_symtable.h"
#include "compiler/symtable/symtable_token.h"
#include "jackc_string.h"
#include "utils.h"
#include <tau.h>
#include <string.h> // memcmp

struct symtab_fixture {
    sym_table* symtab;
};

TEST_F_SETUP(symtab_fixture) {
    tau->symtab = sym_table_init(NULL);
}

TEST_F_TEARDOWN(symtab_fixture) {
    sym_table_free(&tau->symtab);
}

#define REQUIRE_SYMTAB_TOKEN_EQ(name, expected) do { \
    REQUIRE(found); \
    REQUIRE(memcmp(&found, expected, sizeof(sym_table_token)) == 0); \
} while(0)

void require_symtab_token_eq(const sym_table* symtab, const jackc_string* name, const sym_table_token* expected) {
    sym_table_token found;
    REQUIRE(sym_table_find(symtab, name, &found));
    REQUIRE(memcmp(&found, expected, sizeof(sym_table_token)) == 0);
}

TEST_F(symtab_fixture, find_local) {
    sym_table_token token = create_token(JACK_INT, VAR_ARGUMENT, "x", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(tau->symtab, &token));
    require_symtab_token_eq(tau->symtab, &token.name, &token);
}

TEST_F(symtab_fixture, find_multiple_scopes) {
    // Create token "x" in a parent scope
    sym_table_token token = create_token(JACK_INT, VAR_ARGUMENT, "x", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(tau->symtab, &token));

    // Create token "x" in the child scope
    sym_table* top_symtab = sym_table_push(tau->symtab);
    sym_table_token token2 = create_token(JACK_INT, VAR_LOCAL, "x", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(top_symtab, &token2));

    // Expect the child scope token to be found first
    require_symtab_token_eq(top_symtab, &token2.name, &token2);
    // Pop the child scope and expect the parent scope token to be found
    sym_table_pop(top_symtab);
    require_symtab_token_eq(tau->symtab, &token.name, &token);
}

TEST_F(symtab_fixture, exists_local) {
    sym_table_token token = create_token(JACK_INT, VAR_ARGUMENT, "x", "");
    REQUIRE_SYMTAB_OK(sym_table_insert(tau->symtab, &token));

    sym_table* top_symtab = sym_table_push(tau->symtab);

    require_symtab_token_eq(top_symtab, &token.name, &token);
    REQUIRE_FALSE(sym_table_exists_local(top_symtab, &token.name));

    sym_table_pop(top_symtab);
    REQUIRE(sym_table_exists_local(tau->symtab, &token.name));
}
