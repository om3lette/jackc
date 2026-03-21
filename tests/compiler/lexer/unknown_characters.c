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

static bool is_multibyte_char(jack_lexer* lexer, jack_token* token, const char* c) {
    for (;*c; ++c) {
        *token = jack_lexer_next_token(lexer);
        if (!is_token_match(token, *c)) {
            return false;
        }
    }
    return true;
}
#define REQUIRE_MULTIPLE_CHAR_TOKENS(c) REQUIRE(is_multibyte_char(tau->lexer, &tau->token, c))

TEST_F(lexer_fixture, non_ascii_at_the_start_of_the_token) {
    test_jack_lexer_new_buffer(tau->lexer, "дet x;");

    REQUIRE_MULTIPLE_CHAR_TOKENS("д");
    REQUIRE_ID_TOKEN("et");
    REQUIRE_ID_TOKEN("x");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, non_ascii_in_the_middle_of_the_token) {
    test_jack_lexer_new_buffer(tau->lexer, "vаr x;");

    REQUIRE_ID_TOKEN("v");
    REQUIRE_MULTIPLE_CHAR_TOKENS("а");
    REQUIRE_ID_TOKEN("r");
    REQUIRE_ID_TOKEN("x");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, non_ascii_at_the_end_of_the_token) {
    test_jack_lexer_new_buffer(tau->lexer, "leе x;");

    REQUIRE_ID_TOKEN("le");
    REQUIRE_MULTIPLE_CHAR_TOKENS("е");
    REQUIRE_ID_TOKEN("x");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, non_ascii_as_a_separate_token) {
    test_jack_lexer_new_buffer(tau->lexer, "var ю;");

    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    REQUIRE_MULTIPLE_CHAR_TOKENS("ю");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}
