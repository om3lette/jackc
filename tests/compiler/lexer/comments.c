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

static void require_var_x(struct lexer_fixture* tau) {
    REQUIRE_KEYWORD_TOKEN(TOKEN_VAR);
    REQUIRE_ID_TOKEN("x");
    REQUIRE_CHAR_TOKEN(';');
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, single_line_comment_separate_line) {
    test_jack_lexer_new_buffer(tau->lexer, "// One line comment\nvar x;");
    require_var_x(tau);
}

TEST_F(lexer_fixture, single_line_comment_end_of_line) {
    test_jack_lexer_new_buffer(tau->lexer, "var x; // One line comment");
    require_var_x(tau);
}

TEST_F(lexer_fixture, multi_line_comment) {
    test_jack_lexer_new_buffer(tau->lexer, "/* Line #1\nLine #2 */\nvar x;");
    require_var_x(tau);
}

TEST_F(lexer_fixture, one_line_comment_inside_of_multi_line_comment) {
    test_jack_lexer_new_buffer(tau->lexer, "/* // Comment in comment Line #1\nLine #2 */\nvar x;");
    require_var_x(tau);
}

TEST_F(lexer_fixture, multi_line_comment_inside_of_one_line_comment) {
    test_jack_lexer_new_buffer(tau->lexer, "// /* Multi-line comment with no end\nvar x;");
    require_var_x(tau);
}

TEST_F(lexer_fixture, multi_line_comment_inside_of_multi_line_comment) {
    test_jack_lexer_new_buffer(tau->lexer, "/* /* Multi-line comment with no end */\nvar x;");
    require_var_x(tau);
}
