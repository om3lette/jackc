#ifndef JACKC_COMPILER_PARSER_H
#define JACKC_COMPILER_PARSER_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/allocators.h"

typedef struct jack_parser jack_parser;

typedef enum {
    SYNC_CLASS_BODY = (1 << 0), // native field, static, method, function, constructor, }
    SYNC_SUBROUTINE = (1 << 1), // native function, method, constructor, }
    SYNC_STATEMENT  = (1 << 2), // let, if, while, return, do, }
    SYNC_VAR_DEC    = (1 << 3), // , ;
    SYNC_PARAM_LIST = (1 << 4), // , )
} jack_sync_context;

/**
 * Pushes a sync context onto the parser's sync context stack.
 *
 * @param parser The parser.
 * @param context The sync context to push.
 */
void jack_sync_context_push(jack_parser* parser, jack_sync_context context);

/**
 * Pops a sync context from the parser's sync context stack.
 *
 * @param parser The parser.
 * @param context The sync context to pop.
 */
void jack_sync_context_pop(jack_parser* parser, jack_sync_context context);

struct jack_parser {
    jack_lexer* lexer;
    jack_token previous;
    jack_token current;
    jack_token next;

    jackc_diagnostic_engine* engine;

    bool had_error;
    bool panic_mode;
    jack_sync_context sync_context;

    Allocator* allocator;
};

[[ nodiscard ]] jack_parser jack_parser_init(jack_lexer* lexer, jackc_diagnostic_engine* engine, Allocator* allocator);

/**
 * Parses the input and returns the AST class.
 *
 * @param parser The parser.
 * @return The AST class.
 */
[[ nodiscard ]] ast_class* jack_parser_parse(jack_parser* parser);

#endif
