#ifndef JACKC_COMPILER_PARSER_DIAGNOSTICS_H
#define JACKC_COMPILER_PARSER_DIAGNOSTICS_H

#include "compiler/parser/compiler_parser.h"

void jack_parser_diag_unexpected_token(jack_parser* parser, int32_t expected, int32_t got);

void jack_parser_diag_missing_variable_kind(jack_parser* parser);

void jack_parser_diag_invalid_variable_type(jack_parser* parser);

#endif
