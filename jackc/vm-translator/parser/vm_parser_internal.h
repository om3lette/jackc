#ifndef JACKC_VM_PARSER_INTERNAL_H
#define JACKC_VM_PARSER_INTERNAL_H

#include "vm-translator/parser/vm_parser.h"

[[ nodiscard ]] vm_line vm_parser_parse_line(vm_parser* parser);

[[ nodiscard ]] vm_cmd vm_parser_parse_instruction(vm_parser* parser);

/**
 * Returns the type of the current command.
 *
 * @param parser The parser instance.
 */
[[ nodiscard ]] vm_cmd vm_parser_parse_command_type(vm_parser* parser);

/**
 * Returns the first argument of the current command.
 *
 * @param parser The parser instance.
 */
[[ nodiscard ]] vm_first_arg vm_parser_parse_arg1(vm_parser* parser, vm_cmd cmd);

/**
 * Returns the second argument of the current command.
 *
 * @param parser The parser instance.
 */
[[ nodiscard ]] vm_second_arg vm_parser_parse_arg2(vm_parser* parser, vm_cmd cmd);

void vm_parser_skip_one_line_comment(vm_parser* parser);

#endif
