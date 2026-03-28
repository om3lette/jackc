#include "compiler/lexer/compiler_lexer.h"
#include "lexer_test_utils.h"
#include "test_lexer_common.h"
#include "tau.h"

TEST_F_SETUP(lexer_fixture) {
    tau->lexer = jack_lexer_init("");
}

TEST_F_TEARDOWN(lexer_fixture) { (void)tau; }

#define REQUIRE_COL(c) REQUIRE(tau->lexer.col == (c))
#define REQUIRE_LINE(l) REQUIRE(tau->lexer.line == (l))

TEST_F(lexer_fixture, line_pos_initial_values) {
    REQUIRE_COL(LEXER_DEFAULT_COL);
    REQUIRE_LINE(LEXER_DEFAULT_LINE);
}

TEST_F(lexer_fixture, col_single_line) {
    test_jack_lexer_new_buffer(&tau->lexer, "var int g;");

    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    REQUIRE_COL(LEXER_FIRST_COL + 3);
    REQUIRE_LINE(LEXER_DEFAULT_LINE);

    REQUIRE_KEYWORD_TOKEN(TOKEN_INT);
    REQUIRE_COL(LEXER_FIRST_COL + 7);
    REQUIRE_LINE(LEXER_DEFAULT_LINE);

    REQUIRE_ID_TOKEN("g");
    REQUIRE_COL(LEXER_FIRST_COL + 9);
    REQUIRE_LINE(LEXER_DEFAULT_LINE);

    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_COL(LEXER_FIRST_COL + 10);
    REQUIRE_LINE(LEXER_DEFAULT_LINE);

    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, line_counting_lf) {
    test_jack_lexer_new_buffer(&tau->lexer, "\nt\nv");

    REQUIRE_ID_TOKEN("t");
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 1);

    REQUIRE_ID_TOKEN("v");
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 2);
}

TEST_F(lexer_fixture, line_counting_crlf) {
    test_jack_lexer_new_buffer(&tau->lexer, "\r\nt\r\nv");

    REQUIRE_ID_TOKEN("t");
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 1);

    REQUIRE_ID_TOKEN("v");
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 2);
}

TEST_F(lexer_fixture, col_and_line_counting_lf_crlf) {
    test_jack_lexer_new_buffer(&tau->lexer, "\nt t1\r\nv v12");

    REQUIRE_ID_TOKEN("t");
    REQUIRE_COL(LEXER_FIRST_COL + 1);
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 1);

    REQUIRE_ID_TOKEN("t1");
    REQUIRE_COL(LEXER_FIRST_COL + 4);
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 1);

    REQUIRE_ID_TOKEN("v");
    REQUIRE_COL(LEXER_FIRST_COL + 1);
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 2);

    REQUIRE_ID_TOKEN("v12");
    REQUIRE_COL(LEXER_FIRST_COL + 5);
    REQUIRE_LINE(LEXER_DEFAULT_LINE + 2);
}
