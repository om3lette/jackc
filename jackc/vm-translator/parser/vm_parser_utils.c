#include "core/asserts/jackc_assert.h"
#include "vm-translator/parser/vm_parser.h"
#include "vm-translator/parser/vm_parser_utils.h"

bool is_valid_state(vm_parser* parser) {
    return parser->status == VM_OK;
}

const char* vm_get_current_position(const vm_parser* parser) {
    return parser->buffer.data + parser->pos;
}

char vm_parser_peek(const vm_parser* parser) {
    jackc_assert(parser && "Parser is null");
    jackc_assert(parser->pos <= parser->buffer.length && "Buffer index out of range");

    return parser->buffer.data[parser->pos];
}

char vm_parser_peek_next(const vm_parser* parser, size_t offset) {
    jackc_assert(parser && "Parser is null");
    jackc_assert(parser->pos + offset <= parser->buffer.length && "Buffer index out of range");

    return parser->buffer.data[parser->pos + offset];
}

void vm_parser_skip_new_line(vm_parser* parser) {
    vm_parser_skip_crlf(parser);
    vm_parser_skip_lf(parser);
    parser->line_start = vm_get_current_position(parser);
}

void vm_parser_skip_blank(vm_parser* parser) {
    char c = vm_parser_peek(parser);
    while (c == ' ' || c == '\t') {
        ++parser->pos;
        c = vm_parser_peek(parser);
    }
}

void vm_parser_skip_crlf(vm_parser* parser) {
    while (vm_parser_peek(parser) == '\r') {
        ++parser->pos;
        // TODO:
        // JACKC_VM_PARSER_ASSERT(parser, vm_parser_peek(parser) == '\n', "Invalid CRLF sequence. Found '\r' but '\n' did not follow");
        ++parser->pos;
        ++parser->line_idx;
    }
}

void vm_parser_skip_lf(vm_parser* parser) {
    while (vm_parser_peek(parser) == '\n') {
        ++parser->pos;
        ++parser->line_idx;
    }
}

bool is_line_ending(char c) {
    return c == '\n' || c == '\r' || c == '\0';
}

bool vm_cmd_is_arithmetic(vm_cmd cmd_type) {
    switch (cmd_type) {
        case C_ADD:
        case C_SUB:
        case C_NEG:
        case C_MUL:
        case C_DIV:
        case C_EQ:
        case C_GT:
        case C_LT:
        case C_AND:
        case C_OR:
        case C_NOT:
            return true;
        default:
            return false;
    }
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
