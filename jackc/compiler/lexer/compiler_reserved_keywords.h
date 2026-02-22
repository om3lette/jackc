#ifndef JACKC_COMPILER_RESERVED_KEYWORDS_H
#define JACKC_COMPILER_RESERVED_KEYWORDS_H

#include "compiler/lexer/compiler_lexer.h"
#include <stdbool.h>

typedef struct {
    const char* keyword;
    jack_token_type type;
} jack_keyword;

jack_token_type jack_lexer_lookup_keyword(const char* str, size_t length);

#endif
