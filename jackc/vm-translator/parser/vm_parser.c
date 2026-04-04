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
 * VM commands mapping to arguments count.
 *
 * Data type used must be at least 16 bits.
 * Otherwise it can compile to a base64 string in RISC-V.
 * RARS is unable to interpret it correctly.
 *
 * Example riscv32-unknown-elf-gcc output:
 *
 * 	    .type	vm_cmd_to_args, @object
 *	    .size	vm_cmd_to_args, 18
 *	vm_cmd_to_args:
 *	    .base64	"AAAAAAAAAAAAAAICAQEBAgAC"
 *	    .align	2
 */
static const uint16_t vm_cmd_to_args[] = {
    [C_ADD] = 0,
    [C_SUB] = 0,
    [C_NEG] = 0,
    [C_AND] = 0,
    [C_OR] = 0,
    [C_NOT] = 0,
    [C_EQ] = 0,
    [C_GT] = 0,
    [C_LT] = 0,
    [C_PUSH] = 2,
    [C_POP] = 2,
    [C_LABEL] = 1,
    [C_GOTO] = 1,
    [C_IF_GOTO] = 1,
    [C_FUNCTION] = 2,
    [C_RETURN] = 0,
    [C_CALL] = 2
};

static bool is_valid_state(vm_parser* parser) {
    return parser->status == VM_OK;
}

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

vm_cmd jackc_vm_parse_command(vm_parser* parser) {
    jackc_assert(parser && "Parser is null");

    const char* token_start = vm_get_current_position(parser);
    size_t token_size = 0;

    char c = jackc_tolower(vm_parser_peek(parser));
    while ((c >= 'a' && c <= 'z') || c == '-') {
        ++parser->pos;
        ++token_size;
        c = jackc_tolower(vm_parser_peek(parser));
    }

    vm_cmd cmd = jackc_vm_cmd_type_from_string(
        parser, &jackc_string_create(token_start, token_size)
    );
    if (is_valid_state(parser))
        LOG_DEBUG("%s\n", vm_cmd_type_to_string(cmd));

    return cmd;
}

vm_line jackc_vm_parse_line(vm_parser* parser) {
    jackc_assert(parser && "Parser is null.");
    LOG_DEBUG("Line %u\n", parser->line_idx);

    vm_line token = {0};

    token.cmd = jackc_vm_parse_command(parser);
    if (!is_valid_state(parser))
        return token;

    uint16_t cmd_argc = vm_cmd_to_args[token.cmd];
    if (cmd_argc > 0) {
        token.arg1 = vm_parser_parse_arg1(parser, token.cmd);
        if (!is_valid_state(parser))
            return token;
    }
    if (cmd_argc == 2) {
        token.arg2 = vm_parser_parse_arg2(parser, token.cmd);
        if (!is_valid_state(parser))
            return token;
    }
    LOG_DEBUG_NO_BANNER("\n");

    if (
        (token.cmd == C_PUSH || token.cmd == C_POP)
        && token.arg1.segment == SEGMENT_POINTER
        && (token.arg2.value != 1 && token.arg2.value != 1)
    ) {
        parser->status = VM_INVALID_POINTER_IDX;
    }
    return token;
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

    vm_line line = jackc_vm_parse_line(parser);
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
