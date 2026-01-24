#ifndef JACKC_VM_TRANSLATOR_UTILS_H
#define JACKC_VM_TRANSLATOR_UTILS_H

#include "vm-translator/parser.h"

/**
 * Returns a string representation of the given command type.
 *
 * @param cmd_type The command type to convert to a string.
 */
char* vm_cmd_type_to_string(jackc_vm_cmd_type cmd_type);

/**
 * Returns true if the given command type is an arithmetic command.
 *
 * @param cmd_type The command type to check.
 */
bool vm_cmd_is_arithmetic(jackc_vm_cmd_type cmd_type);

/**
 * Returns a pointer to the current position of the parser.
 *
 * @param parser The parser instance.
 */
const char* vm_get_current_position(const jackc_parser* parser);

/**
 * Returns the current character of the parser.
 *
 * @param parser The parser instance.
 */
char vm_parser_peek(const jackc_parser* parser);

/**
 * Returns the next character of the parser.
 *
 * @param parser The parser instance.
 * @param offset The offset to add to the current position.
 */
char vm_parser_peek_next(const jackc_parser* parser, size_t offset);

/**
 * Skips new lines: CRLF, LF.
 * Updates `parser->line_start`.
 *
 * @param parser The parser instance.
 */
void vm_parser_skip_new_line(jackc_parser* parser);

/**
 * Skips whitespaces and tabs.
 *
 * @param parser The parser instance.
 */
void vm_parser_skip_blank(jackc_parser* parser);

/**
 * Skips new lines: CRLF
 *
 * @param parser The parser instance.
 */
void vm_parser_skip_crlf(jackc_parser* parser);

/**
 * Skips new lines: LF
 *
 * @param parser The parser instance.
 */
void vm_parser_skip_lf(jackc_parser* parser);

/**
 * Returns true if the given character is a line ending.
 * '\\n', '\r' or '\0'
 *
 * @param c The character to check.
 */
bool is_line_ending(char c);
#endif
