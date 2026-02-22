#ifndef JACKC_COMPILER_LEXER_H
#define JACKC_COMPILER_LEXER_H

#include "jackc_string.h"
#include <stdint.h>

typedef int32_t jack_int;

#define LEXER_DEFAULT_CHAR ' '

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
    VAR_VAR = 0,
    VAR_ARGUMENT,
    VAR_STATIC,
    VAR_FIELD
} jack_variable_type;

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
