#ifndef JACKC_COMPILER_LEXER_H
#define JACKC_COMPILER_LEXER_H

#include "compiler/diagnostics-engine/diagnostic.h"
#include "std/jackc_string.h"
#include <stdint.h>

typedef int32_t jack_int;

#define LEXER_DEFAULT_CHAR ' '
#define LEXER_DEFAULT_LINE 1

typedef enum {
    OK = 0
} jackc_lexer_return_code;

typedef enum {
    TOKEN_EOF = 0, // Match '\0'
    TOKEN_CLASS = 256,
    TOKEN_CONSTRUCTOR,
    TOKEN_FUNCTION,
    TOKEN_METHOD,
    TOKEN_FIELD,
    TOKEN_STATIC,
    TOKEN_VAR,
    TOKEN_INT,
    TOKEN_CHAR,
    TOKEN_BOOLEAN,
    TOKEN_VOID,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_NATIVE,
    TOKEN_THIS,
    TOKEN_LET,
    TOKEN_DO,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_INT_LITERAL,
    TOKEN_STR_LITERAL,
    TOKEN_IDENTIFIER,
} jack_token_type;

typedef enum {
    VAR_STATIC = 0,
    VAR_FIELD,
    VAR_LOCAL,
    VAR_ARG,
} jack_variable_kind;

typedef union {
    jack_int integer;
} jack_token_value;

typedef struct {
    int32_t type;
    jackc_string str;
    jack_token_value value;
    jackc_span span;
} jack_token;

typedef struct {
    jackc_string buffer;
    uint32_t pos, line, col;
    char c;
} jack_lexer;

static inline jackc_string jack_token_span_to_string(const jack_token* token, const jackc_string* buffer) {
    return (jackc_string){ .data = buffer->data + token->span.start, .length = token->span.end - token->span.start };
}

static inline jackc_string jack_token_span_from_lexer(const jack_token* token, const jack_lexer* lexer) {
    return (jackc_string){ .data = lexer->buffer.data + token->span.start, .length = token->span.end - token->span.start };
}

[[nodiscard]] jack_lexer jack_lexer_init(const char* buffer);

[[nodiscard]] jack_token jack_lexer_next_token(jack_lexer* lexer);

[[nodiscard]] bool jack_lexer_has_token(jack_lexer* lexer);

#endif
