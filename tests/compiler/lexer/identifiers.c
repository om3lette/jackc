#include "compiler/lexer/compiler_lexer.h"
#include "lexer_test_utils.h"
#include "test_lexer_common.h"
#include "tau.h"

TEST_F_SETUP(lexer_fixture) {
    tau->lexer = jack_lexer_init("");
}

TEST_F_TEARDOWN(lexer_fixture) { (void)tau; }

TEST_F(lexer_fixture, identifier_starting_with_underscore) {
    test_jack_lexer_new_buffer(&tau->lexer, "static int _var_with_underscore");
    REQUIRE_KEYWORD_TOKEN(TOKEN_STATIC);
    REQUIRE_KEYWORD_TOKEN(TOKEN_INT);
    REQUIRE_ID_TOKEN("_var_with_underscore");
    REQUIRE_END_TOKEN();
}
