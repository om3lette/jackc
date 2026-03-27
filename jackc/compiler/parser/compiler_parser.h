#ifndef JACKC_COMPILER_PARSER_H
#define JACKC_COMPILER_PARSER_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/allocators.h"

typedef struct jack_parser jack_parser;

typedef enum {
    SYNC_CLASS_BODY = (1 << 0), // field, static, method, function, constructor, }
    SYNC_SUBROUTINE = (1 << 1), // function, method, constructor, }
    SYNC_STATEMENT  = (1 << 2), // let, if, while, return, do, }
    SYNC_VAR_DEC    = (1 << 3), // , ;
    SYNC_PARAM_LIST = (1 << 4), // , )
} jack_sync_context;

void jack_sync_context_push(jack_parser* parser, jack_sync_context context);
void jack_sync_context_pop(jack_parser* parser, jack_sync_context context);

struct jack_parser {
    jack_lexer* lexer;
    jack_token current;
    jack_token next;

    jackc_diagnostic_engine* engine;
    int32_t previous_token_type; // Used for error recovery

    bool had_error;
    bool panic_mode;
    jack_sync_context sync_context;

    Allocator* allocator;
};

[[ nodiscard ]] jack_parser* jack_parser_init(jack_lexer* lexer, jackc_diagnostic_engine* engine, Allocator* allocator);

[[ nodiscard ]] ast_class* jack_parser_parse(jack_parser* parser);

#endif
