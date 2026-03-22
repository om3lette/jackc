#ifndef JACKC_COMPILER_PARSER_H
#define JACKC_COMPILER_PARSER_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/allocators.h"

typedef struct {
    jack_lexer* lexer;
    jack_token current;
    jack_token next;

    jack_diagnostic_engine* engine;
    int32_t previous_token_type; // Used for error recovery

    bool had_error;
    bool panic_mode;

    Allocator* allocator;
} jack_parser;

[[ nodiscard ]] jack_parser* jack_parser_init(jack_lexer* lexer, jack_diagnostic_engine* engine, Allocator* allocator);

[[ nodiscard ]] ast_class* jack_parser_parse(jack_parser* parser);

#endif
