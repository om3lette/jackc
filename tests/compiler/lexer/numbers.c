#include "compiler/lexer/compiler_lexer.h"
#include "jackc_stdlib.h"
#include "lexer_test_utils.h"
#include "tau.h"

TEST_F_SETUP(lexer_fixture) {
    tau->lexer = jack_lexer_init("");
}

TEST_F_TEARDOWN(lexer_fixture) {
    jackc_free(tau->lexer);
}

TEST_F(lexer_fixture, positive_integer) {
    test_jack_lexer_new_buffer(tau->lexer, "var int g = 123;");
    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    REQUIRE_KEYWORD_TOKEN(TOKEN_INT);
    REQUIRE_ID_TOKEN("g");
    REQUIRE_CHAR_TOKEN('=');
    REQUIRE_INT_TOKEN(123);
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, zero_integer) {
    test_jack_lexer_new_buffer(tau->lexer, "var int g = 0;");
    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    REQUIRE_KEYWORD_TOKEN(TOKEN_INT);
    REQUIRE_ID_TOKEN("g");
    REQUIRE_CHAR_TOKEN('=');
    REQUIRE_INT_TOKEN(0);
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, negative_integer) {
    test_jack_lexer_new_buffer(tau->lexer, "var int g = -123;");
    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    REQUIRE_KEYWORD_TOKEN(TOKEN_INT);
    REQUIRE_ID_TOKEN("g");
    REQUIRE_CHAR_TOKEN('=');
    REQUIRE_CHAR_TOKEN('-');
    REQUIRE_INT_TOKEN(123);
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}
