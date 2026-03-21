#ifndef JACKC_COMPILER_PARSER_H
#define JACKC_COMPILER_PARSER_H

#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/allocators.h"

typedef struct {
    jack_lexer lexer;
    jack_token current;
    jack_token next;
    Allocator allocator;
} jack_parser;

[[ nodiscard ]] jack_parser* jack_parser_init(jack_lexer* lexer, Allocator* allocator);

#endif
