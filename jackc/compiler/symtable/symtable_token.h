#ifndef JACKC_SYMTABLE_TOKEN_H
#define JACKC_SYMTABLE_TOKEN_H

#include "compiler/lexer/compiler_lexer.h"
#include "jackc_string.h"

typedef enum {
    JACK_INT = 0,
    JACK_BOOLEAN,
    JACK_CHAR,
    JACK_USER_DEFINED
} jack_type;

typedef struct {
    jack_variable_kind kind;
    uint32_t idx;
} jack_variable;

typedef struct {
    jack_type type;
    jack_variable var;
    jackc_string name;
    jackc_string str_type;
} sym_table_token;

/**
 * Creates a new `sym_table_token`. Will shallow copy provided strings.
 * Initializes the variable index to 0. It is expected that a symbol table will assign a unique index to each variable.
 *
 * @param type The type of the token.
 * @param var_type The variable type of the token.
 * @param name The name of the token.
 * @param str_type The string type of the token.
 * @return A new sym_table_token.
 */
static inline sym_table_token sym_table_token_init(
    jack_type type,
    jack_variable_kind var_type,
    const jackc_string* name,
    const jackc_string* str_type
) {
    sym_table_token token;
    token.type = type;
    token.var.kind = var_type;
    token.var.idx = 0;
    // Shallow copies
    token.name = *name;
    token.str_type = *str_type;
    return token;
}

#endif
