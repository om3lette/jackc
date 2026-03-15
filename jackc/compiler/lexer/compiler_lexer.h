#ifndef JACKC_COMPILER_LEXER_H
#define JACKC_COMPILER_LEXER_H

#include "jackc_string.h"
#include <stdint.h>

typedef int32_t jack_int;

#define LEXER_DEFAULT_CHAR ' '
// Has similar semantics to lexer->pos
// Will display index of current char to CONSUME (which is + 1 from the current char)
// Therefore 0 means 1 indexed (i -> i + 1 indexed)
#define LEXER_DEFAULT_COL 0
#define LEXER_FIRST_COL (LEXER_DEFAULT_COL + 1)
#define LEXER_DEFAULT_LINE 1

typedef enum {
    OK = 0
} jackc_lexer_return_code;

typedef enum {
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
    TOKEN_THIS,
    TOKEN_LET,
    TOKEN_DO,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_NUMBER_INT,
    TOKEN_IDENTIFIER,
    TOKEN_EOF
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
    uint32_t line, col;
} jack_location;

typedef struct {
    int32_t type;
    jackc_string str;
    jack_token_value value;
    jack_location loc;
} jack_token;

typedef struct {
    jackc_string buffer;
    uint32_t pos, line, col;
    char c;
} jackc_lexer;

[[nodiscard]] jackc_lexer* jack_lexer_init(const char* buffer);

[[nodiscard]] jack_token jack_lexer_next_token(jackc_lexer* lexer);

// [[nodiscard]] jackc_lexer_return_code jack_lexer_peek();

#endif
