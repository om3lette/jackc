#include "vm_parser.h"
#include "core/asserts/jackc_assert.h"
#include "core/logging/logger.h"
#include "std/jackc_stdio.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"
#include "vm-translator/backend.h"
#include "vm-translator/parser/vm_parser_internal.h"
#include "vm-translator/parser/vm_parser_utils.h"
#include <stdio.h>

/**
 * Implementation of jackc_parser_init function.
 */
vm_parser vm_parser_init(const jackc_string* buffer) {
    vm_parser parser = {
        .buffer = *buffer,
        .pos = 0,
        .status = VM_OK,
        .line_idx = 0,
        .line_start = buffer->data,
    };
    vm_parser_advance(&parser); // Initialize .current
    if (!is_valid_state(&parser))
        return parser;
    vm_parser_advance(&parser); // Initialize .next
    return parser;
}

bool vm_parser_has_more_lines(vm_parser* parser) {
    jackc_assert(parser->next.line_start);
    return parser->next.line_start != (parser->buffer.data + parser->buffer.length);
}

void vm_parser_advance(vm_parser* parser) {
    while (
        !jackc_isdigit(vm_parser_peek(parser))
        && !jackc_isalpha(vm_parser_peek(parser))
        && !vm_parser_check(parser, '\0')
    ) {
        vm_parser_skip_new_line(parser);
        vm_parser_skip_blank(parser);
        vm_parser_skip_one_line_comment(parser);
    }

    vm_line line;
    if (vm_parser_check(parser, '\0')) {
        // Dummy node to continue the cycle of prev / current / next
        // `vm_parser_has_more_lines` will report no more lines available
        line.line_start = vm_parser_current_position(parser);
    } else {
        line = vm_parser_parse_line(parser);
        // Fail fast
        if (!is_valid_state(parser))
            return;
    }

    parser->current = parser->next;
    parser->next = line;

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
