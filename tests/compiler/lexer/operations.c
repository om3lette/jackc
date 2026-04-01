#include "compiler/lexer/compiler_lexer.h"
#include "lexer_test_utils.h"
#include "test_lexer_common.h"
#include "tau.h"

TEST_F_SETUP(lexer_fixture) {
    tau->lexer = jack_lexer_init("");
}

TEST_F_TEARDOWN(lexer_fixture) { (void)tau; }

TEST_F(lexer_fixture, division_op) {
    test_jack_lexer_new_buffer(&tau->lexer, "2 / 4");
    REQUIRE_INT_TOKEN(2);
    REQUIRE_CHAR_TOKEN('/');
    REQUIRE_INT_TOKEN(4);
    REQUIRE_END_TOKEN();
}
