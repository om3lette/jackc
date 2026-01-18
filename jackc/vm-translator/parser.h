#ifndef JACKC_PARSER_H
#define JACKC_PARSER_H

/**
 * All possible Jack VM command types.
 */
#include <stdint.h>
typedef enum {
    C_ARITHMETIC = 0,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    C_CONST
} jackc_vm_cmd_type;

/**
 * Parser struct for jackc.
 *
 * @todo Consider writing a `string_view`
 */
#include <stddef.h>
typedef struct {
    const char* buffer; /**< Input file buffer to parse. */
    const char* line_start; /**< Pointer to the current line start. */
    size_t position; /**< Current position in the buffer. */
} jackc_parser;

/**
 * Allocates memory for a new jackc_parser instance.
 *
 * Initializes the parser with the given buffer and default values
 *
 * @param buffer The input file buffer to parse.
 * @return A pointer to the newly allocated jackc_parser instance.
 */
[[ nodiscard ]] jackc_parser* jackc_parser_init(const char* buffer);

/**
 * Checks if there are more lines to parse.
 *
 * @param parser The parser instance.
 * @return True if there are more lines, false otherwise.
 */
[[ nodiscard ]] bool jackc_parser_has_more_lines(const jackc_parser* parser);

/**
 * Advances the parser to the next line.
 *
 * @param parser The parser instance.
 */
void jackc_parser_advance(jackc_parser* parser);

/**
 * Returns the type of the current command.
 *
 * @param parser The parser instance.
 * @return The type of the current command.
 */
[[ nodiscard ]] jackc_vm_cmd_type jackc_parser_command_type(const jackc_parser* parser);

/**
 * Returns the first argument of the current command.
 *
 * @param parser The parser instance.
 * @return The first argument of the current command.
 */
[[ nodiscard ]] const char* jackc_parser_arg1(const jackc_parser* parser);

/**
 * Returns the second argument of the current command.
 *
 * @param parser The parser instance.
 * @return The second argument of the current command.
 */
[[ nodiscard ]] const char* jackc_parser_arg2(const jackc_parser* parser);

/**
 * Returns the second argument of the current command as an integer.
 *
 * @param parser The parser instance.
 * @return The second argument of the current command.
 */
[[ nodiscard ]] int32_t jackc_parser_arg2_int(const jackc_parser* parser);

#endif
