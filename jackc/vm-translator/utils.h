#ifndef JACKC_VM_TRANSLATOR_UTILS_H
#define JACKC_VM_TRANSLATOR_UTILS_H

#include "vm-translator/parser.h"

char* jackc_cmd_type_to_string(jackc_vm_cmd_type cmd_type);

bool jackc_vm_cmd_is_arithmetic(jackc_vm_cmd_type cmd_type);

const char* get_current_position(const jackc_parser* parser);

char vm_parser_peek(const jackc_parser* parser);

char vm_parser_peek_next(const jackc_parser* parser, size_t offset);

void vm_parser_skip_new_line(jackc_parser* parser);

/**
 * Skips whitespaces and tabs.
 */
void vm_parser_skip_blank(jackc_parser* parser);

void vm_parser_skip_crlf(jackc_parser* parser);

void vm_parser_skip_lf(jackc_parser* parser);

bool is_line_ending(char c);
#endif
