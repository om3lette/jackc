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

typedef enum {
    SEGMENT_THIS,
    SEGMENT_THAT,
    SEGMENT_ARG,
    SEGMENT_LOCAL,
    SEGMENT_CONSTANT,
    SEGMENT_STATIC,
    SEGMENT_TEMP,
    SEGMENT_POINTER
} jackc_vm_segment_type;

/**
 * Parser struct for jackc.
 */
 typedef struct {
     size_t position; /**< Current position in the buffer. */
     const char* line_start; /**< Pointer to the current line start. */

     jackc_string buffer; /**< Input file buffer to parse. */
     jackc_string arg1; /**< First argument of the current command. */

     uint32_t line_idx; /**< Line index of the current command. Starting from 1. */
     jackc_vm_cmd_type cmd; /**< Type of the current command. */
     jackc_vm_segment_type segment; /**< Segment type. Valid if cmd is C_PUSH or C_POP. Parser version of arg1 */
     int32_t arg2; /**< Second argument of the current command. */

     bool is_arg1_set; /**< Indicates if the first argument is set. */
     bool is_arg2_set; /**< Indicates if the second argument is set. */
 } jackc_parser;

/**
 * Allocates memory for a new jackc_parser instance.
 *
 * Initializes the parser with the given buffer and default values
 *
 * @param buffer The input file buffer to parse.
 */
[[ nodiscard ]] jackc_parser* jackc_parser_init(const char* buffer);

void jackc_parser_update_source(jackc_parser* parser, const char* buffer);

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
 */
[[ nodiscard ]] bool jackc_parser_has_more_lines(jackc_parser* parser);

/**
 * Advances the parser to the next valid instruction.
 * Skips comments and blank lines.
 *
 * @param parser The parser instance.
 */
void jackc_vm_parser_advance(jackc_parser* parser);

/**
 * Returns the type of the current command.
 *
 * @param parser The parser instance.
 */
[[ nodiscard ]] jackc_vm_cmd_type jackc_parser_command_type(const jackc_parser* parser);

/**
 * Returns the first argument of the current command.
 *
 * @param parser The parser instance.
 */
[[ nodiscard ]] const char* jackc_parser_arg1(const jackc_parser* parser);

/**
 * Returns the second argument of the current command.
 *
 * @param parser The parser instance.
 */
[[ nodiscard ]] const char* jackc_parser_arg2(const jackc_parser* parser);

/**
 * Prints debug information and the current parser state.
 *
 * @param parser The parser instance.
 * @param msg The message to print.
 * @param c_file The file name where the error occurred.
 * @param c_line The line number where the error occurred.
 */
void jackc_vm_parser_panic(const jackc_parser* parser, const char* msg, const char* c_file, unsigned int c_line);

#define JACKC_VM_PARSER_ASSERT(parser, condition, message) \
    do { \
        if (!(condition)) { \
            jackc_vm_parser_panic(parser, message, __FILE__, __LINE__); \
        } \
    } while(0)

#endif
