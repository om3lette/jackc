#include "parser.h"
#include "common/exit_code.h"
#include "common/logger.h"
#include "common/jackc_assert.h"
#include "jackc_stdlib.h"
#include "jackc_string.h"
#include "vm-translator/utils.h"
#include <inttypes.h>
#include <stdio.h>

static const uint8_t vm_cmd_to_args[18] = {
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
    [C_CALL] = 2,
    [C_UNKNOWN] = 0
};

/**
 * Implementation of jackc_parser_init function.
 */
jackc_parser* jackc_parser_init(const char *buffer) {
    jackc_parser* parser = jackc_alloc(sizeof(jackc_parser));
    parser->buffer = jackc_string_create(buffer, jackc_strlen(buffer));
    parser->line_start = parser->buffer.data;
    parser->line_idx = 1;
    parser->position = 0;

    parser->arg1 = jackc_string_empty();
    parser->arg2 = 0;

    parser->is_arg1_set = false;
    parser->is_arg2_set = false;

    return parser;
}

const char* get_current_position(const jackc_parser* parser) {
    return parser->buffer.data + parser->position;
}

char peek(const jackc_parser* parser) {
    jackc_assert(parser && "Parser is null");
    jackc_assert(parser->position <= parser->buffer.length && "Buffer index out of range");

    return parser->buffer.data[parser->position];
}

/**
 * Implementation of jackc_parser_free function.
 */
void jackc_parser_free(jackc_parser* parser) {
    jackc_free((void*)parser->buffer.data);
    jackc_free(parser);
}

/**
 * Skips whitespaces and tabs.
 */
void jackc_vm_parser_skip_blank(jackc_parser* parser) {
    char c = peek(parser);
    while (c == ' ' || c == '\t') {
        ++parser->position;
        c = peek(parser);
    }
}

bool jackc_parser_has_more_lines(jackc_parser* parser) {
    jackc_vm_parser_skip_blank(parser);

    char c = peek(parser);
    JACKC_VM_PARSER_ASSERT(parser, parser->line_idx == 1 || c == '\0' || c == '\r' || c == '\n', "Unexpected character");

    return c != '\0';
}

jackc_vm_cmd_type jackc_vm_cmd_type_from_string(const jackc_string str) {
    if (jackc_strcmp(&str, "add")) return C_ADD;
    if (jackc_strcmp(&str, "sub")) return C_SUB;
    if (jackc_strcmp(&str, "neg")) return C_NEG;
    if (jackc_strcmp(&str, "eq")) return C_EQ;
    if (jackc_strcmp(&str, "gt")) return C_GT;
    if (jackc_strcmp(&str, "lt")) return C_LT;
    if (jackc_strcmp(&str, "and")) return C_AND;
    if (jackc_strcmp(&str, "or")) return C_OR;
    if (jackc_strcmp(&str, "not")) return C_NOT;
    if (jackc_strcmp(&str, "push")) return C_PUSH;
    if (jackc_strcmp(&str, "pop")) return C_POP;
    if (jackc_strcmp(&str, "label")) return C_LABEL;
    if (jackc_strcmp(&str, "goto")) return C_GOTO;
    if (jackc_strcmp(&str, "if-goto")) return C_IF_GOTO;
    if (jackc_strcmp(&str, "function")) return C_FUNCTION;
    if (jackc_strcmp(&str, "return")) return C_RETURN;
    if (jackc_strcmp(&str, "call")) return C_CALL;
    return C_UNKNOWN;
}

/**
 * @todo Support comments?
 * @todo Remove unknown command from jackc_vm_cmd_type?
 */
jackc_vm_cmd_type jackc_vm_parse_command(jackc_parser* parser) {
    jackc_assert(parser && "Parser is null");

    const char* token_start = get_current_position(parser);
    size_t token_size = 0;

    char c = jackc_tolower(peek(parser));
    while ((c >= 'a' && c <= 'z') || c == '-') {
        ++parser->position;
        ++token_size;
        c = jackc_tolower(peek(parser));
    }

    jackc_vm_cmd_type cmd = jackc_vm_cmd_type_from_string(
        jackc_string_create(token_start, token_size)
    );
    JACKC_VM_PARSER_ASSERT(parser, parser->cmd != C_UNKNOWN, "Unknown command");
    parser->cmd = cmd;
    LOG_DEBUG("%s\n", jackc_cmd_type_to_string(parser->cmd));

    return cmd;
}

void jackc_vm_parse_arg1(jackc_parser* parser) {
    jackc_assert(parser && "Parser is null.");

    // TODO: Make a separate function
    bool is_invalid_cmd_type = parser->cmd == C_RETURN || parser->cmd == jackc_vm_cmd_is_arithmetic(parser->cmd);
    jackc_assert(!is_invalid_cmd_type && "Invalid command type");

    jackc_vm_parser_skip_blank(parser);

    const char* token_start = get_current_position(parser);
    size_t token_size = 0;

    char c = jackc_tolower(peek(parser));
    while ((c >= 'a' && c <= 'z') || c == '_' || c == '.') {
        ++parser->position;
        ++token_size;
        c = jackc_tolower(peek(parser));
    }
    JACKC_VM_PARSER_ASSERT(parser, token_size, "Token size is zero");

    parser->arg1 = jackc_string_create(token_start, token_size);
    parser->is_arg1_set = true;

    LOG_DEBUG("%.*s\n", token_size, parser->arg1.data);

    return;
}

void jackc_vm_parse_arg2(jackc_parser* parser) {
    jackc_assert(parser && "Parser is null.");

    // TODO: Make a separate function
    bool is_valid_cmd_type =
        parser->cmd == C_PUSH
        || parser->cmd == C_POP
        || parser->cmd == C_FUNCTION
        || parser->cmd == C_CALL;
    jackc_assert(is_valid_cmd_type && "Invalid command type");

    jackc_vm_parser_skip_blank(parser);

    const char* token_start = get_current_position(parser);
    size_t token_size = 0;

    char c = peek(parser);
    while ((c >= '0' && c <= '9')) {
        ++parser->position;
        ++token_size;
        c = peek(parser);
    }


    JACKC_VM_PARSER_ASSERT(parser, token_size, "Token size is zero");
    jackc_string token = jackc_string_create(token_start, token_size);

    parser->arg2 = jackc_atoi(&token);
    parser->is_arg2_set = true;
    LOG_DEBUG("%d\n", parser->arg2);

    return;
}

void jackc_vm_parse_line(jackc_parser* parser) {
    jackc_assert(parser && "Parser is null.");
    LOG_DEBUG("Line %u\n", parser->line_idx);

    jackc_vm_cmd_type cmd = jackc_vm_parse_command(parser);
    uint8_t cmd_argc = vm_cmd_to_args[cmd];

    if (cmd_argc > 0)  jackc_vm_parse_arg1(parser);
    if (cmd_argc == 2) jackc_vm_parse_arg2(parser);

    LOG_DEBUG_NO_BANNER("\n");

    return;
}

void jackc_vm_parser_advance(jackc_parser* parser) {
    JACKC_VM_PARSER_ASSERT(parser, peek(parser) != '\0', "Unexpected EOF");

    // First line
    if (parser->position == 0) {
        jackc_vm_parse_line(parser);
        return;
    }

    jackc_vm_parser_skip_blank(parser);
    JACKC_VM_PARSER_ASSERT(parser, peek(parser) == '\r' || peek(parser) == '\n', "Failed to reach end of line");

    // TODO: Make a separate function
    // Handle CRLF
    while (peek(parser) == '\r') {
        ++parser->position;
        JACKC_VM_PARSER_ASSERT(parser, peek(parser) == '\n', "Found '\r' but '\n' did not follow.");
        ++parser->position;
        ++parser->line_idx;
    }

    // TODO: Make a separate function
    // Handle LF
    while (peek(parser) == '\n') {
        ++parser->position;
        ++parser->line_idx;
    }

    parser->line_start = get_current_position(parser);
    parser->is_arg1_set = false;
    parser->is_arg2_set = false;

    if (peek(parser) == '\0') return;
    jackc_vm_parse_line(parser);
    return;
}

void jackc_vm_parser_panic(const jackc_parser* parser, const char* msg, const char* c_file, unsigned int c_line) {
    jackc_printf("\n\n");
    LOG_FATAL("Invalid VM syntax at line %d\n", parser->line_idx);
    LOG_FATAL("Current symbol index: %d\n\n", parser->line_start - parser->buffer.data);
    const char* cur = parser->line_start;
    while (*cur != '\n' && *cur != '\0') {
        jackc_printf("%c", *cur++);
    }
    jackc_printf("\n\n%s at %s:%d\n", msg, c_file, c_line);

    jackc_exit(JACKC_ASSERTION_ERROR);
}
