#ifndef JACKC_TESTS_COMPILER_LEXER_TEST_UTILS_H
#define JACKC_TESTS_COMPILER_LEXER_TEST_UTILS_H

#include "compiler/lexer/compiler_lexer.h"

#define NEXT_TOKEN() do { \
    tau->token = jack_lexer_next_token(tau->lexer); \
} while(0)

#define REQUIRE_END_TOKEN() do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_match(&tau->token, '\0')); \
} while(0)

#define REQUIRE_NEXT_TOKEN(token_chr) do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_match(&tau->token, token_chr)); \
} while(0)

#define REQUIRE_NEXT_TOKEN_STR(token_type, token_str) do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_equal_str(&tau->token, token_type, token_str)); \
} while(0)

#define REQUIRE_NEXT_TOKEN_INT(token_value) do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_equal_int(&tau->token, token_value)); \
} while(0)


[[nodiscard]] bool is_token_equal_str(const jack_token* token, int token_type, const char* token_value);

[[nodiscard]] bool is_token_equal_int(const jack_token* token, jack_int token_value);

[[nodiscard]] bool is_token_match(const jack_token* token, char chr);

void test_jack_lexer_new_buffer(jackc_lexer* lexer, const char* buffer);

#endif
