#include "core/asserts/jackc_assert.h"
#include "vm-translator/parser/vm_parser.h"
#include "vm-translator/parser/vm_parser_utils.h"

bool is_valid_state(vm_parser* parser) {
    return parser->status == VM_OK;
}

const char* vm_parser_current_position(const vm_parser* parser) {
    return parser->buffer.data + parser->pos;
}

char vm_parser_peek(const vm_parser* parser) {
    jackc_assert(parser && "Parser is null");
    jackc_assert(parser->pos <= parser->buffer.length && "Buffer index out of range");

    return parser->buffer.data[parser->pos];
}

char vm_parser_peek_next(const vm_parser* parser) {
    jackc_assert(parser && "Parser is null");
    jackc_assert(parser->pos + 1 <= parser->buffer.length && "Buffer index out of range");

    return parser->buffer.data[parser->pos + 1];
}

bool vm_parser_check(const vm_parser* parser, char c) {
    return vm_parser_peek(parser) == c;
}

bool vm_parser_match(vm_parser* parser, char c) {
    if (vm_parser_check(parser, c)) {
        ++parser->pos;
        return true;
    }
    return false;
}

void vm_parser_skip_new_line(vm_parser* parser) {
    // CRLF
    while (vm_parser_match(parser, '\r')) {
        if (vm_parser_match(parser, '\n')) {
            ++parser->line_idx;
            parser->line_start = vm_parser_current_position(parser);
        }
    }
    // LF
    while (vm_parser_match(parser, '\n')) {
        ++parser->line_idx;
        parser->line_start = vm_parser_current_position(parser);
    }
}

void vm_parser_skip_blank(vm_parser* parser) {
    while (
        vm_parser_match(parser, ' ')
        || vm_parser_match(parser, '\t')
    ) {}
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

char* vm_cmd_type_to_string(vm_cmd cmd_type) {
    switch (cmd_type) {
        case C_ADD:
            return "add";
        case C_SUB:
            return "sub";
        case C_NEG:
            return "neg";
        case C_MUL:
            return "mul";
        case C_DIV:
            return "div";
        case C_EQ:
            return "eq";
        case C_GT:
            return "gt";
        case C_LT:
            return "lt";
        case C_AND:
            return "and";
        case C_OR:
            return "or";
        case C_NOT:
            return "not";
        case C_PUSH:
            return "push";
        case C_POP:
            return "pop";
        case C_LABEL:
            return "label";
        case C_GOTO:
            return "goto";
        case C_IF_GOTO:
            return "if-goto";
        case C_FUNCTION:
            return "function";
        case C_RETURN:
            return "return";
        case C_CALL:
            return "call";
    }
    return "unknown";
}
