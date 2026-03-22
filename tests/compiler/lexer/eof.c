#include "compiler/lexer/compiler_lexer.h"
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

TEST_F(lexer_fixture, empty_string_returns_eof) {
    test_jack_lexer_new_buffer(tau->lexer, "");

    REQUIRE_CHAR_TOKEN('\0');
}

TEST_F(lexer_fixture, empty_string_returns_endless_eof) {
    test_jack_lexer_new_buffer(tau->lexer, "");

    for (int i = 0; i < 100; i++) {
        REQUIRE_CHAR_TOKEN('\0');
    }
}

TEST_F(lexer_fixture, endless_eof_after_string_consumption) {
    test_jack_lexer_new_buffer(tau->lexer, "var");

    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    for (int i = 0; i < 100; i++) {
        REQUIRE_CHAR_TOKEN('\0');
    }
}
