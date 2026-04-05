#ifndef JACKC_VM_TRANSLATOR_UTILS_H
#define JACKC_VM_TRANSLATOR_UTILS_H

#include "vm-translator/parser/vm_parser.h"

[[ nodiscard ]] vm_cmd jackc_vm_cmd_type_from_string(vm_parser* parser, const jackc_string* str);

[[ nodiscard ]] bool is_valid_state(vm_parser* parser);

/**
 * Returns a string representation of the given command type.
 *
 * @param cmd_type The command type to convert to a string.
 */
char* vm_cmd_type_to_string(vm_cmd cmd_type);

/**
 * Returns a pointer to the current position of the parser.
 *
 * @param parser The parser instance.
 */
const char* vm_parser_current_position(const vm_parser* parser);

/**
 * Returns the current character of the parser.
 *
 * @param parser The parser instance.
 */
char vm_parser_peek(const vm_parser* parser);

/**
 * Returns the next character of the parser.
 *
 * @param parser The parser instance.
 */
char vm_parser_peek_next(const vm_parser* parser);

/**
 * Skips new lines: CRLF, LF.
 * Updates `parser->line_start`.
 *
 * @param parser The parser instance.
 */
void vm_parser_skip_new_line(vm_parser* parser);

/**
 * Skips whitespaces and tabs.
 *
 * @param parser The parser instance.
 */
void vm_parser_skip_blank(vm_parser* parser);

[[ nodiscard ]] bool vm_parser_check(const vm_parser* parser, char c);

[[ nodiscard ]] bool vm_parser_match(vm_parser* parser, char c);

#endif
