#include "parser.h"
#include "jackc_stdlib.h"
#include <inttypes.h>

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

[[ nodiscard ]] bool jackc_parser_has_more_lines(const jackc_parser* parser) {
    (void)parser;
    return false;
}

void jackc_parser_advance(jackc_parser* parser) {
    (void)parser;
    return;
}

[[ nodiscard ]] jackc_vm_cmd_type jackc_parser_command_type(const jackc_parser* parser) {
    (void)parser;
    return 0;
}

[[ nodiscard ]] const char* jackc_parser_arg1(const jackc_parser* parser) {
    (void)parser;
    return 0;
}

[[ nodiscard ]] const char* jackc_parser_arg2(const jackc_parser* parser) {
    (void)parser;
    return 0;
}

[[ nodiscard ]] int32_t jackc_parser_arg2_int(const jackc_parser* parser) {
    (void)parser;
    return 0;
}
