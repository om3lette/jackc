#include "compiler/lexer/compiler_lexer.h"
#include "jackc_stdlib.h"
#include "lexer_test_utils.h"
#include "tau.h"

struct lexer_internal_fixture {
    jackc_lexer* lexer;
    jack_token token;
};

TEST_F_SETUP(lexer_internal_fixture) {
    tau->lexer = jack_lexer_init("");
}

TEST_F_TEARDOWN(lexer_internal_fixture) {
    jackc_free(tau->lexer);
}

TEST_F(lexer_internal_fixture, base_case) {
    test_jack_lexer_new_buffer(tau->lexer, "var int g;");

    REQUIRE_NEXT_TOKEN_STR(TOKEN_VAR, "var");
    REQUIRE_NEXT_TOKEN_STR(TOKEN_INT, "int");
    REQUIRE_NEXT_TOKEN_STR(TOKEN_IDENTIFIER, "g");
    REQUIRE_NEXT_TOKEN(';');
    REQUIRE_END_TOKEN();
}


TEST_F(lexer_internal_fixture, lexer_ignores_spaces) {
    test_jack_lexer_new_buffer(tau->lexer, "  let  numerator   =    x  ;  ");

    REQUIRE_NEXT_TOKEN_STR(TOKEN_LET, "let");
    REQUIRE_NEXT_TOKEN_STR(TOKEN_IDENTIFIER, "numerator");
    REQUIRE_NEXT_TOKEN('=');
    REQUIRE_NEXT_TOKEN_STR(TOKEN_IDENTIFIER, "x");
    REQUIRE_NEXT_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_internal_fixture, lexer_ignores_spac) {
    test_jack_lexer_new_buffer(tau->lexer, "  let  nюmerator   =    x  ;  ");

    REQUIRE_NEXT_TOKEN_STR(TOKEN_LET, "let");
    REQUIRE_NEXT_TOKEN_STR(TOKEN_IDENTIFIER, "n");
    // TODO: Write a utility
    // 'ю'
    REQUIRE_NEXT_TOKEN(-47);
    REQUIRE_NEXT_TOKEN(-114);
    REQUIRE_NEXT_TOKEN_STR(TOKEN_IDENTIFIER, "merator");
    REQUIRE_NEXT_TOKEN('=');
    REQUIRE_NEXT_TOKEN_STR(TOKEN_IDENTIFIER, "x");
    REQUIRE_NEXT_TOKEN(';');
    REQUIRE_END_TOKEN();
}
