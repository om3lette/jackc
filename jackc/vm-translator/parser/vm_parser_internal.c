#include "vm_parser_internal.h"
#include "core/asserts/jackc_assert.h"
#include "core/logging/logger.h"
#include "std/jackc_string.h"
#include "vm-translator/parser/vm_parser.h"
#include "vm-translator/parser/vm_parser_utils.h"
#include "std/jackc_stdlib.h"

#define RETURN_IF_INVALID_STATE(_parser, _return_value) do { \
    if (!is_valid_state(_parser)) \
        return _return_value; \
} while (0)

#define RETURN_WITH_STATUS(_parser, _status, _ret_val) do { \
    _parser->status = _status; \
    return _ret_val; \
} while (0)

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

vm_line vm_parser_parse_line(vm_parser* parser) {
    jackc_assert(parser && "Parser is null.");
    LOG_DEBUG("Line %u\n", parser->line_idx);

    vm_line token = {0};

    token.cmd = vm_parser_parse_instruction(parser);
    RETURN_IF_INVALID_STATE(parser, token);

    uint16_t cmd_argc = vm_cmd_to_args[token.cmd];
    if (cmd_argc > 0) {
        token.arg1 = vm_parser_parse_arg1(parser, token.cmd);
        RETURN_IF_INVALID_STATE(parser, token);
    }
    if (cmd_argc == 2) {
        token.arg2 = vm_parser_parse_arg2(parser, token.cmd);
        RETURN_IF_INVALID_STATE(parser, token);
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

vm_cmd vm_parser_parse_instruction(vm_parser* parser) {
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


vm_first_arg vm_parser_parse_arg1(vm_parser* parser, vm_cmd cmd) {
    jackc_assert(parser && "Parser is null");

    vm_first_arg arg = {0};

    vm_parser_skip_blank(parser);
    const char* token_start = vm_get_current_position(parser);
    size_t token_size = 0;

    char c = jackc_tolower(vm_parser_peek(parser));
    while ((c >= 'a' && c <= 'z') || c == '_' || c == '.' || (c >= '0' && c <= '9')) {
        ++parser->pos;
        ++token_size;
        c = jackc_tolower(vm_parser_peek(parser));
    }
    if (token_size <= 0)
        RETURN_WITH_STATUS(parser, VM_EMPTY_FIRST_ARGUMENT, arg);

    jackc_string arg1_str = jackc_string_create(token_start, token_size);
    arg.str = arg1_str;

    LOG_DEBUG("%.*s\n", token_size, token_start);
    if (cmd == C_PUSH || cmd == C_POP) {
        if (jackc_streq(&arg1_str, "this")) {
            arg.segment = SEGMENT_THIS;
        } else if (jackc_streq(&arg1_str, "that")) {
            arg.segment = SEGMENT_THAT;
        } else if (jackc_streq(&arg1_str, "local")) {
            arg.segment = SEGMENT_LOCAL;
        } else if (jackc_streq(&arg1_str, "argument")) {
            arg.segment = SEGMENT_ARG;
        } else if (jackc_streq(&arg1_str, "static")) {
            arg.segment = SEGMENT_STATIC;
        } else if (jackc_streq(&arg1_str, "constant")) {
            arg.segment = SEGMENT_CONSTANT;
        } else if (jackc_streq(&arg1_str, "temp")) {
            arg.segment = SEGMENT_TEMP;
        } else if (jackc_streq(&arg1_str, "pointer")) {
            arg.segment = SEGMENT_POINTER;
        } else {
            RETURN_WITH_STATUS(parser, VM_INVALID_SEGMENT, arg);
        }
    }
    if (cmd == C_POP && arg.segment == SEGMENT_CONSTANT)
        RETURN_WITH_STATUS(parser, VM_POP_SEGMENT_CONST, arg);
    return arg;
}

vm_second_arg vm_parser_parse_arg2(vm_parser* parser, vm_cmd cmd) {
    jackc_assert(parser && "Parser is null");

    vm_second_arg arg = {0};
    // TODO: Make a separate function
    bool is_valid_cmd_type =
        cmd == C_PUSH
        || cmd == C_POP
        || cmd == C_FUNCTION
        || cmd == C_CALL;
    if (!is_valid_cmd_type)
        RETURN_WITH_STATUS(parser,VM_INVALID_CMD, arg);

    vm_parser_skip_blank(parser);

    const char* token_start = vm_get_current_position(parser);
    size_t token_size = 0;

    char c = vm_parser_peek(parser);
    if (c == '-') ++parser->pos, ++token_size;
    c = vm_parser_peek(parser);
    while ((c >= '0' && c <= '9')) {
        ++parser->pos;
        ++token_size;
        c = vm_parser_peek(parser);
    }

    if (token_size <= 0)
        RETURN_WITH_STATUS(parser, VM_EMPTY_SECOND_ARGUMENT, arg);
    jackc_string token = jackc_string_create(token_start, token_size);

    arg.str = token;
    arg.value = jackc_atoi(&token);

    if (
        arg.value == 0
        && (
            (token.data[0] == '-' && (token.length == 1 || token.data[1] != '0'))
            || token.data[0] != '0'
        )
    ) {
        RETURN_WITH_STATUS(parser, VM_INVALID_ARG_2, arg);
    }
    LOG_DEBUG("%d\n", arg.value);

    return arg;
}

bool vm_parser_skip_one_line_comment(vm_parser* parser) {
    jackc_assert(parser && "Parser is null");
    bool reached_eol = false;
    vm_parser_skip_blank(parser);

    if (vm_parser_peek(parser) == '/' && vm_parser_peek_next(parser, 1) == '/') {
        while (!(reached_eol = is_line_ending(vm_parser_peek(parser)))) {
            ++parser->pos;
        }
    }

    return reached_eol;
}
