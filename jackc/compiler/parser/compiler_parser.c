#include "compiler_parser.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/allocators.h"
#include "jackc_stdlib.h"

jack_parser* jack_parser_init(jack_lexer* lexer, Allocator* allocator) {
    jack_parser* parser = allocator->alloc(sizeof(Allocator), allocator->context);

    jackc_memcpy(&parser->allocator, allocator, sizeof(Allocator));
    jackc_memcpy(&parser->lexer, lexer, sizeof(jack_lexer));

    return parser;
}
