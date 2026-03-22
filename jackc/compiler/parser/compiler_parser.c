#include "compiler_parser.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/allocators.h"

jack_parser* jack_parser_init(jack_lexer* lexer, jack_diagnostic_engine* engine, Allocator* allocator) {
    jack_parser* parser = allocator->alloc(sizeof(jack_parser), allocator->context);

    parser->allocator = allocator;
    parser->engine = engine;
    parser->lexer = lexer;

    parser->had_error = false;
    parser->panic_mode = false;

    parser->current = jack_lexer_next_token(parser->lexer);
    parser->next = jack_lexer_next_token(parser->lexer);
    parser->previous_token_type = TOKEN_EOF;

    return parser;
}
