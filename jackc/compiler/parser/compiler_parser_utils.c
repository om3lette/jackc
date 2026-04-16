#include "compiler_parser_utils.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser_internal.h"

bool is_statement_start(jack_parser* parser) {
    return jack_parser_check(parser, TOKEN_LET)
        || jack_parser_check(parser, TOKEN_DO)
        || jack_parser_check(parser, TOKEN_IF)
        || jack_parser_check(parser, TOKEN_WHILE)
        || jack_parser_check(parser, TOKEN_RETURN);
}

bool is_subroutine_start(jack_parser* parser) {
    return jack_parser_check(parser, TOKEN_FUNCTION)
        || jack_parser_check(parser, TOKEN_METHOD)
        || jack_parser_check(parser, TOKEN_CONSTRUCTOR)
        || jack_parser_check(parser, TOKEN_NATIVE);
}

bool is_class_var_start(jack_parser* parser) {
    return jack_parser_check(parser, TOKEN_STATIC)
        || jack_parser_check(parser, TOKEN_FIELD);
}

bool is_class_member_start(jack_parser* parser) {
    return is_class_var_start(parser)
        || is_subroutine_start(parser);
}

bool is_rcurl_or_eof(jack_parser* parser) {
    return jack_parser_check(parser, '}')
        || jack_parser_check(parser, TOKEN_EOF);
}

void enter_panic_mode(jack_parser* parser) {
    parser->had_error = true;
    parser->panic_mode = true;
}

void exit_panic_mode(jack_parser* parser) {
    parser->panic_mode = false;
}

bool is_panic_mode(jack_parser* parser) {
    return parser->panic_mode;
}

jackc_diag_builder diagnostic_begin(const jack_parser* parser, jackc_diagnostic_severity severity, jackc_diagnostic_code code) {
    return jackc_diag_begin(parser->engine, severity, code, parser->current.span);
}
