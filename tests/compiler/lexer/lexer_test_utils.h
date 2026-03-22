#ifndef JACKC_TESTS_COMPILER_LEXER_TEST_UTILS_H
#define JACKC_TESTS_COMPILER_LEXER_TEST_UTILS_H

#include "compiler/lexer/compiler_lexer.h"
#include "compiler/lexer/compiler_reserved_keywords.h" // IWYU pragma: keep

struct lexer_fixture {
    jack_lexer* lexer;
    jack_token token;
};

#define NEXT_TOKEN() do { \
    tau->token = jack_lexer_next_token(tau->lexer); \
} while(0)

#define REQUIRE_END_TOKEN() do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_match(&tau->token, '\0')); \
} while(0)

#define REQUIRE_CHAR_TOKEN(token_chr) do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_match(&tau->token, token_chr)); \
} while(0)

#define REQUIRE_ID_TOKEN(token_str) do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_equal_str(&tau->token, TOKEN_IDENTIFIER, token_str)); \
} while(0)

#define REQUIRE_KEYWORD_TOKEN(keyword_type) do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_equal_str(&tau->token, keyword_type, jack_lexer_get_keyword(keyword_type))); \
} while(0)

#define REQUIRE_INT_TOKEN(token_value) do { \
    NEXT_TOKEN(); \
    REQUIRE(is_token_equal_int(&tau->token, token_value)); \
} while(0)


[[nodiscard]] bool is_token_equal_str(const jack_token* token, int token_type, const char* token_value);

[[nodiscard]] bool is_token_equal_int(const jack_token* token, jack_int token_value);

[[nodiscard]] bool is_token_match(const jack_token* token, char chr);

#endif
