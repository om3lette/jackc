#include "parser.h"
#include "jackc_stdlib.h"

/**
 * Implementation of jackc_parser_init function.
 */
jackc_parser* jackc_parser_init(const char *buffer) {
    jackc_parser* parser = jackc_alloc(sizeof(jackc_parser));
    parser->buffer = buffer;
    parser->line_start = buffer;
    parser->position = 0;
    return parser;
}

/**
 * Implementation of jackc_parser_free function.
 */
void jackc_parser_free(jackc_parser* parser) {
    jackc_free((void*)parser->buffer);
    jackc_free(parser);
}
