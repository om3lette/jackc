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

TEST_F(lexer_fixture, string_literal_letters) {
    test_jack_lexer_new_buffer(tau->lexer, "\"test\"");
    REQUIRE_STR_LITERAL_TOKEN("test");
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, string_literal_starting_with_numbers) {
    test_jack_lexer_new_buffer(tau->lexer, "\"123test\"");
    REQUIRE_STR_LITERAL_TOKEN("123test");
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, string_literal_special_characters) {
    test_jack_lexer_new_buffer(tau->lexer, "\"%&;/<>|[$#(}_*){-+]~\"");
    REQUIRE_STR_LITERAL_TOKEN("%&;/<>|[$#(}_*){-+]~");
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, string_literal_spaces) {
    test_jack_lexer_new_buffer(tau->lexer, "\"sp ac\te\"");
    REQUIRE_STR_LITERAL_TOKEN("sp ac\te");
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, string_literal_eof) {
    test_jack_lexer_new_buffer(tau->lexer, "\"123test");
    REQUIRE_STR_LITERAL_TOKEN("123test");
    REQUIRE_END_TOKEN();
}

TEST_F(lexer_fixture, string_literal_line_break) {
    test_jack_lexer_new_buffer(tau->lexer, "\"123test\nnew_string\"");
    REQUIRE_STR_LITERAL_TOKEN("123test");
    REQUIRE_ID_TOKEN("new_string");
    REQUIRE_STR_LITERAL_TOKEN("");
    REQUIRE_END_TOKEN();
}
