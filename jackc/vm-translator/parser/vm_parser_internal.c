#include "vm_parser_internal.h"
#include "core/asserts/jackc_assert.h"
#include "core/logging/logger.h"
#include "std/jackc_string.h"
#include "vm-translator/parser/vm_parser.h"
#include "vm-translator/parser/vm_parser_utils.h"

#define RETURN_WITH_STATUS(_parser, _status, _ret_val) do { \
    _parser->status = _status; \
    return _ret_val; \
} while(0)

vm_first_arg vm_parser_parse_arg1(vm_parser* parser, vm_cmd cmd) {
    jackc_assert(parser && "Parser is null");

    vm_first_arg arg = {0};
    if (cmd == C_RETURN || cmd == vm_cmd_is_arithmetic(cmd)) {
        RETURN_WITH_STATUS(parser, VM_INVALID_CMD, arg);
    }

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
    while ((c >= '0' && c <= '9') || c == '-') {
        ++parser->pos;
        ++token_size;
        c = vm_parser_peek(parser);
    }

    if (token_size <= 0)
        RETURN_WITH_STATUS(parser, VM_EMPTY_SECOND_ARGUMENT, arg);

    jackc_string token = jackc_string_create(token_start, token_size);

    arg.str = token;
    arg.value = jackc_atoi(&token);
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

vm_cmd jackc_vm_cmd_type_from_string(vm_parser* parser, const jackc_string* str) {
    if (jackc_streq(str, "add")) return C_ADD;
    if (jackc_streq(str, "sub")) return C_SUB;
    if (jackc_streq(str, "neg")) return C_NEG;
    if (jackc_streq(str, "mul")) return C_MUL;
    if (jackc_streq(str, "div")) return C_DIV;
    if (jackc_streq(str, "eq")) return C_EQ;
    if (jackc_streq(str, "gt")) return C_GT;
    if (jackc_streq(str, "lt")) return C_LT;
    if (jackc_streq(str, "and")) return C_AND;
    if (jackc_streq(str, "or")) return C_OR;
    if (jackc_streq(str, "not")) return C_NOT;
    if (jackc_streq(str, "push")) return C_PUSH;
    if (jackc_streq(str, "pop")) return C_POP;
    if (jackc_streq(str, "label")) return C_LABEL;
    if (jackc_streq(str, "goto")) return C_GOTO;
    if (jackc_streq(str, "if-goto")) return C_IF_GOTO;
    if (jackc_streq(str, "function")) return C_FUNCTION;
    if (jackc_streq(str, "return")) return C_RETURN;
    if (jackc_streq(str, "call")) return C_CALL;

    parser->status = VM_INVALID_CMD;
    return C_EQ;
}
