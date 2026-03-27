#ifndef JACKC_COMPILER_PARSER_UTILS_H
#define JACKC_COMPILER_PARSER_UTILS_H

#include "compiler/parser/compiler_parser.h"

[[ nodiscard ]] bool is_statement_start(jack_parser* parser);
[[ nodiscard ]] bool is_subroutine_start(jack_parser* parser);
[[ nodiscard ]] bool is_class_var_start(jack_parser* parser);
[[ nodiscard ]] bool is_rcurl_or_eof(jack_parser* parser);

void enter_panic_mode(jack_parser* parser);
void exit_panic_mode(jack_parser* parser);
[[ nodiscard ]] bool is_panic_mode(jack_parser* parser);
[[ nodiscard ]] jackc_span token_to_span(const jack_parser* parser, jack_token token);
[[ nodiscard ]] jackc_diag_builder diagnostic_begin(const jack_parser* parser, jackc_diagnostic_severity severity, jackc_diagnostic_code code);


#endif
