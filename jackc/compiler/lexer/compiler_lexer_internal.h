#ifndef JACKC_COMPILER_LEXER_INTERNAL_H
#define JACKC_COMPILER_LEXER_INTERNAL_H

#include "compiler/lexer/compiler_lexer.h"
#include <stdint.h>

const char* jack_lexer_pos(const jackc_lexer* lexer, int32_t offset);

const char* jack_lexer_cur_pos(const jackc_lexer* lexer);

void jack_lexer_read_char(jackc_lexer* lexer);

void jack_lexer_skip_blank_and_comments(jackc_lexer* lexer);

[[nodiscard]] bool jack_lexer_read_and_expect(jackc_lexer* lexer, char expected);

[[nodiscard]] jack_token jack_lexer_new_str_token(jackc_lexer* lexer, int32_t type, const char* start);

[[nodiscard]] jack_token jack_lexer_new_int_token(jackc_lexer* lexer, const char* start, jack_int value);

#endif
