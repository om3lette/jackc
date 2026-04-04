#include "vm_parser.h"
#include "core/logging/logger.h"
#include "core/asserts/jackc_assert.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"
#include "vm-translator/backend.h"
#include "vm-translator/parser/vm_parser_internal.h"
#include "vm-translator/parser/vm_parser_utils.h"
#include <stdio.h>

/**
 * Implementation of jackc_parser_init function.
 */
vm_parser jackc_parser_init(const jackc_string* buffer) {
    return (vm_parser) {
        .buffer = *buffer,
        .pos = 0,
        .status = VM_OK,
        .line_idx = 0,
        .line_start = buffer->data
    };
}

void jackc_parser_update_source(vm_parser* parser, const jackc_string* buffer) {
    jackc_assert(parser && "Parser is null");

    parser->buffer = *buffer;
    parser->pos = 0;
    parser->status = VM_OK;
    parser->line_idx = 0;
    parser->line_start = buffer->data;
}


bool vm_parser_has_more_lines(vm_parser* parser) {
    char c = vm_parser_peek(parser);
    vm_parser_skip_blank(parser);
    return c != '\0';
}

void vm_parser_advance(vm_parser* parser) {
    if (vm_parser_peek(parser) != '\0') {
        parser->status = VM_UNEXPECTED_EOF;
        return;
    }

    // Skip any number of comment lines before next instruction.
    vm_parser_skip_new_line(parser); // todo: move to init? This is only needed for the first line.
    while (true) {
        vm_parser_skip_blank(parser);
        bool reached_eol = vm_parser_skip_one_line_comment(parser);
        if (!reached_eol) break;

        vm_parser_skip_new_line(parser);
    }

    if (vm_parser_peek(parser) == '\0') return;

    vm_line line = vm_parser_parse_line(parser);
    if (!is_valid_state(parser))
        return;
    parser->prev = parser->current;
    parser->current = parser->next;
    parser->next = line;

    // Allow comments after the line.
    vm_parser_skip_blank(parser);
    vm_parser_skip_one_line_comment(parser);
    vm_parser_skip_new_line(parser);
    return;
}

void vm_parser_panic(const vm_parser* parser, const char* msg, const char* c_file, unsigned int c_line) {
    jackc_printf("\n\n");
    LOG_FATAL("Invalid VM syntax at line %d\n", parser->line_idx);
    LOG_FATAL("Current symbol index: %d\n\n", parser->pos - (size_t)(parser->line_start - parser->buffer.data));
    const char* cur = parser->line_start;
    while (*cur != '\n' && *cur != '\0') {
        jackc_printf("%c", *cur++);
    }
    jackc_printf("\n\n%s at %s:%d\n", msg, c_file, c_line);

    jackc_exit(BACKEND_INVALID_SYNTAX);
}
