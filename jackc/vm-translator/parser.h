#ifndef JACKC_PARSER_H
#define JACKC_PARSER_H

#include "jackc_string.h"
#include <stdint.h>
#include <stddef.h>

/**
 * All possible Jack VM command types.
 */
typedef enum {
    C_UNKNOWN = 0,
    C_ADD,
    C_SUB,
    C_NEG,
    C_AND,
    C_OR,
    C_NOT,
    C_EQ,
    C_GT,
    C_LT,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF_GOTO,
    C_FUNCTION,
    C_RETURN,
    C_CALL

} jackc_vm_cmd_type;

static const uint8_t vm_cmd_to_args[] = {
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
 * Parser struct for jackc.
 *
 * flags:
 * 0. Indicates if an error occurred during parsing of the current line.
 * 1. Indicates if at least one error [][]occurred during parsing of the entire buffer.
 *
 * Fields are ordered in a way to minimize memory footprint.
 * @todo Consider writing a `string_view`
 */
 typedef struct {
     size_t position; /**< Current position in the buffer. */
     jackc_string buffer; /**< Input file buffer to parse. */
     jackc_string arg1;

     unsigned int line_idx; /**< Pointer to the current line start. */
     jackc_vm_cmd_type cmd;
     int32_t arg2;

     bool is_arg1_set;
     bool is_arg2_set;
     // 2 bytes padding
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
 * Frees the memory allocated for a jackc_parser instance.
 *
 * Invalidates the pointer.
 *
 * @param parser The parser instance to free.
 */
void jackc_parser_free(jackc_parser* parser);

/**
 * Checks if there are more lines to parse.
 *
 * @param parser The parser instance.
 * @return True if there are more lines, false otherwise.
 */
[[ nodiscard ]] bool jackc_parser_has_more_lines(jackc_parser* parser);

/**
 * Advances the parser to the next line.
 *
 * @param parser The parser instance.
 */
void jackc_vm_parser_advance(jackc_parser* parser);

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

void jackc_vm_parser_panic(const jackc_parser* parser, const char* msg, const char* c_file, unsigned int c_line);

#define JACKC_VM_PARSER_ASSERT(parser, condition, message) \
    do { \
        if (!(condition)) { \
            jackc_vm_parser_panic(parser, message, __FILE__, __LINE__); \
        } \
    } while(0)

#endif
