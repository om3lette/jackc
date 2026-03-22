#include "compiler_parser_diagnostics.h"

void jack_parser_diag_unexpected_token(
    jack_parser* parser,
    int32_t expected,
    int32_t got
) {
    jack_diagnostic_push(parser->engine, (jack_diagnostic){
        .loc = parser->current.loc,
        .severity = DIAG_SEV_ERROR,
        .code = DIAG_UNEXPECTED_TOKEN,
        .data = {
            .unexpected_token = {
                .expected = expected,
                .got = got
            }
        }
    });
}

void jack_parser_diag_missing_variable_kind(jack_parser* parser) {
    jack_diagnostic_push(parser->engine, (jack_diagnostic){
        .loc = parser->current.loc,
        .severity = DIAG_SEV_ERROR,
        .code = DIAG_MISSING_VARIABLE_KIND
    });
}

void jack_parser_diag_invalid_variable_type(jack_parser* parser) {
    jack_diagnostic_push(parser->engine, (jack_diagnostic){
        .loc = parser->current.loc,
        .severity = DIAG_SEV_ERROR,
        .code = DIAG_INVALID_VARIABLE_TYPE
    });
}
