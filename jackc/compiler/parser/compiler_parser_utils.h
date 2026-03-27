#ifndef JACKC_COMPILER_PARSER_UTILS_H
#define JACKC_COMPILER_PARSER_UTILS_H

#include "compiler/parser/compiler_parser.h"

[[ nodiscard ]] bool is_statement_start(jack_parser* parser);
[[ nodiscard ]] bool is_subroutine_start(jack_parser* parser);
[[ nodiscard ]] bool is_class_var_start(jack_parser* parser);
[[ nodiscard ]] bool is_rcurl_or_eof(jack_parser* parser);

#endif
