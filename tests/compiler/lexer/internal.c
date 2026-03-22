#include "compiler/lexer/compiler_lexer.h"
#include "compiler/lexer/compiler_lexer_internal.h"
#include "jackc_stdlib.h"
#include "lexer_test_utils.h"
#include "test_lexer_common.h"
#include "tau.h"

TEST_F_SETUP(lexer_fixture) {
    tau->lexer = jack_lexer_init("");
}

TEST_F_TEARDOWN(lexer_fixture) {
    jackc_free(tau->lexer);
}

TEST_F(lexer_fixture, read_char_happy_path) {
    test_jack_lexer_new_buffer(tau->lexer, "a");

    jack_lexer_read_char(tau->lexer);
    REQUIRE(tau->lexer->c == 'a' && "Current char is incorrect");
    REQUIRE(tau->lexer->col == LEXER_DEFAULT_COL + 1 && "col was not updated");
    REQUIRE(tau->lexer->pos == 1 && "pos was not updated");

    jack_lexer_read_char(tau->lexer);
    REQUIRE(tau->lexer->c == '\0' && "Current char is incorrect");
    REQUIRE(tau->lexer->col == LEXER_DEFAULT_COL + 2 && "col was not updated");
    REQUIRE(tau->lexer->pos == 2 && "pos was not updated");
}

TEST_F(lexer_fixture, read_and_expect_happy_path) {
    test_jack_lexer_new_buffer(tau->lexer, "*");

    REQUIRE(jack_lexer_read_and_expect(tau->lexer, '*'));
    REQUIRE(tau->lexer->c == '*' && "Current char was not updated");
}

TEST_F(lexer_fixture, read_and_expect_fail) {
    test_jack_lexer_new_buffer(tau->lexer, "+");

    REQUIRE(!jack_lexer_read_and_expect(tau->lexer, '*'));
    REQUIRE(tau->lexer->c == LEXER_DEFAULT_CHAR && "Current char was not restored.");
    REQUIRE(tau->lexer->pos == 0 && "Position was not restored.");
}
