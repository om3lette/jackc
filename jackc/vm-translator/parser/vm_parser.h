#ifndef JACKC_VM_PARSER_H
#define JACKC_VM_PARSER_H

#include "std/jackc_string.h"
#include <stdint.h>
#include <stddef.h>

/**
 * All possible Jack VM command types.
 */
typedef enum {
    C_ADD = 0,
    C_SUB,
    C_DIV,
    C_MUL,
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
} vm_cmd;

typedef enum {
    SEGMENT_THIS,
    SEGMENT_THAT,
    SEGMENT_ARG,
    SEGMENT_LOCAL,
    SEGMENT_CONSTANT,
    SEGMENT_STATIC,
    SEGMENT_TEMP,
    SEGMENT_POINTER
} vm_segment;

typedef enum {
    VM_OK = 0,
    VM_INVALID_CMD = 2000,
    VM_INVALID_ARG_1,
    VM_INVALID_SEGMENT,
    VM_EMPTY_FIRST_ARGUMENT,
    VM_EMPTY_SECOND_ARGUMENT,
    VM_POP_SEGMENT_CONST,
    VM_INVALID_POINTER_IDX,
    VM_INVALID_ARG_2,
    VM_UNEXPECTED_EOF
} vm_parser_return_code;

typedef struct {
    jackc_string str;
    vm_segment segment;
} vm_first_arg;

typedef struct {
    jackc_string str;
    int32_t value;
} vm_second_arg;

typedef struct {
    const char* line_start;
    uint32_t line_idx;
    vm_cmd cmd;
    vm_first_arg arg1;
    vm_second_arg arg2;
} vm_line;

/**
 * Parser struct for jackc.
 */
 typedef struct {
     jackc_string buffer; /**< Input file buffer to parse. */
     size_t pos; /**< Current position in the buffer. */

     const char* line_start;
     size_t line_idx;

     vm_line prev;
     vm_line current;
     vm_line next;

     vm_parser_return_code status;
 } vm_parser;

/**
 * Allocates memory for a new jackc_parser instance.
 *
 * Initializes the parser with the given buffer and default values
 *
 * @param buffer The input file buffer to parse.
 */
[[ nodiscard ]] vm_parser jackc_parser_init(const jackc_string* buffer);

void jackc_parser_update_source(vm_parser* parser, const jackc_string* buffer);

/**
 * Checks if there are more lines to parse.
 *
 * @param parser The parser instance.
 */
[[ nodiscard ]] bool vm_parser_has_more_lines(vm_parser* parser);

/**
 * Advances the parser to the next valid instruction.
 * Skips comments and blank lines.
 *
 * @param parser The parser instance.
 */
void vm_parser_advance(vm_parser* parser);

#endif
