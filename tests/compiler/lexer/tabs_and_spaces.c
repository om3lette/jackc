#include "compiler/lexer/compiler_lexer.h"
#include "lexer_test_utils.h"
#include "test_lexer_common.h"
#include "tau.h"

TEST_F_SETUP(lexer_fixture) {
    tau->lexer = jack_lexer_init("");
}

TEST_F_TEARDOWN(lexer_fixture) { (void)tau; }

TEST_F(lexer_fixture, lexer_ignores_spaces) {
    test_jack_lexer_new_buffer(&tau->lexer, "  let  numerator   =    x  ;  ");

    REQUIRE_KEYWORD_TOKEN(TOKEN_LET);
    REQUIRE_ID_TOKEN("numerator");
    REQUIRE_CHAR_TOKEN('=');
    REQUIRE_ID_TOKEN("x");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, lexer_ignores_tabs) {
    test_jack_lexer_new_buffer(&tau->lexer, "var\tnumerator\t\t;");

    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    REQUIRE_ID_TOKEN("numerator");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, lexer_ignores_tabs_and_spaces) {
    test_jack_lexer_new_buffer(&tau->lexer, "  let \t  numerator  \t = \t\t   x  \t;  ");

    REQUIRE_KEYWORD_TOKEN(TOKEN_LET);
    REQUIRE_ID_TOKEN("numerator");
    REQUIRE_CHAR_TOKEN('=');
    REQUIRE_ID_TOKEN("x");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}
