#include "compiler_parser.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "core/allocators/allocators.h"
#include "core/asserts/jackc_assert.h"

jack_parser jack_parser_init(jack_lexer* lexer, jackc_diagnostic_engine* engine, Allocator* allocator) {
    jack_parser parser;

    parser.allocator = allocator;
    parser.engine = engine;
    parser.lexer = lexer;

    parser.had_error = false;
    parser.panic_mode = false;
    parser.sync_context = 0;

    parser.current = jack_lexer_next_token(parser.lexer);
    parser.next = jack_lexer_next_token(parser.lexer);
    parser.previous = parser.current;

    return parser;
}

void jack_sync_context_push(jack_parser* parser, jack_sync_context context) {
    jackc_assert(parser && "Parser is null");
    parser->sync_context |= context;
}

void jack_sync_context_pop(jack_parser* parser, jack_sync_context context) {
    jackc_assert(parser && "Parser is null");
    parser->sync_context &= ~context;
}

// This acts as a public api to avoid exposing internal parsing functions
ast_class* jack_parser_parse(jack_parser* parser) {
    return jack_parser_parse_class(parser);
}
