#include "compiler_reserved_keywords.h"
#include "core/asserts/jackc_assert.h"
#include "compiler/lexer/compiler_lexer.h"
#include "std/jackc_string.h"
#include <stddef.h>

static const jack_keyword keywords[] = {
    {"class",       TOKEN_CLASS},
    {"constructor", TOKEN_CONSTRUCTOR},
    {"function",    TOKEN_FUNCTION},
    {"method",      TOKEN_METHOD},
    {"field",       TOKEN_FIELD},
    {"static",      TOKEN_STATIC},
    {"var",         TOKEN_VAR},
    {"int",         TOKEN_INT},
    {"char",        TOKEN_CHAR},
    {"boolean",     TOKEN_BOOLEAN},
    {"void",        TOKEN_VOID},
    {"true",        TOKEN_TRUE},
    {"false",       TOKEN_FALSE},
    {"null",        TOKEN_NULL},
    {"native",      TOKEN_NATIVE},
    {"this",        TOKEN_THIS},
    {"let",         TOKEN_LET},
    {"do",          TOKEN_DO},
    {"if",          TOKEN_IF},
    {"else",        TOKEN_ELSE},
    {"while",       TOKEN_WHILE},
    {"return",      TOKEN_RETURN},
};
static const size_t KEYWORD_CNT = sizeof(keywords) / sizeof(keywords[0]);

const char* jack_lexer_get_keyword(jack_token_type keyword) {
    for (size_t i = 0; i < KEYWORD_CNT; i++) {
        if (keywords[i].type == keyword) {
            return keywords[i].keyword;
        }
    }

    return NULL;
}

/**
 * Lookup a keyword and return its token type.
 *
 * @param str The identifier string to check.
 * @return The corresponding jack_token_type, or TOKEN_IDENTIFIER if not a keyword.
 */
jack_token_type jack_lexer_lookup_keyword(const char* str, size_t length) {
    jackc_assert(str != NULL && "Lookup string cannot be NULL");

    jackc_string s = jackc_string_create(str, length);
    for (size_t i = 0; i < KEYWORD_CNT; i++) {
        if (jackc_streq(&s, keywords[i].keyword)) {
            return keywords[i].type;
        }
    }

    return TOKEN_IDENTIFIER;
}
