#ifndef JACKC_COMPILER_DIAGNOSTICS_ENGINE_DIAGNOSTIC_H
#define JACKC_COMPILER_DIAGNOSTICS_ENGINE_DIAGNOSTIC_H

#include "std/jackc_string.h"
#include <stdint.h>

#define MAX_DIAGNOSTICS 128

typedef struct {
    uint32_t start;
    uint32_t end;
} jackc_span;

typedef enum {
    DIAG_ERROR,
    DIAG_WARNING,
    DIAG_NOTE
} jackc_diagnostic_severity;

typedef enum {
    // Parser
    DIAG_UNEXPECTED_TOKEN,
    DIAG_MISSING_VARIABLE_KIND,
    DIAG_MISSING_VARIABLE_NAME,
    DIAG_INVALID_VARIABLE_TYPE,
    DIAG_INVALID_RETURN_TYPE,
    DIAG_INVALID_SUBROUTINE_KIND,
    DIAG_INVALID_TOKEN_CLASS_BODY,
    DIAG_INVALID_TOKEN_TERM,
    DIAG_MISSING_SEMICOLON,

    // Symtab
    DIAG_REDEFINITION,
    DIAG_INCOMPLETE_TYPE,
    DIAG_USE_OF_UNDECLARED_IDENTIFIER,
    DIAG_CALL_TO_UNDECLARED_SUBROUTINE,
    DIAG_NON_VOID_SUBROUTINE_SHOULD_RETURN_A_VALUE,
    DIAG_CALLED_OBJECT_TYPE_IS_NOT_A_CLASS,

    DIAG_EMPTY_IF_STATEMENT,
    DIAG_INVALID_OPERATION,
    DIAG_CANNOT_CALL_METHOD_WITHOUT_AN_OBJECT,

    // Notes
    DIAG_PREVIOUS_DEFINITION_IS_HERE,

    NUMBER_OF_DIAGNOSTICS,
} jackc_diagnostic_code;

typedef struct jackc_diagnostic jackc_diagnostic;
struct jackc_diagnostic {
    jackc_span span;
    jackc_diagnostic_severity severity;
    jackc_diagnostic_code code;

    jackc_diagnostic* note; // Optional note

    union {
        struct {
            int32_t expected;
            jackc_string got;
        } unexpected_token;
        struct {
            jackc_string got;
        } invalid_token;
        struct {
            jackc_string token;
        } last_valid_token;
    } data;
};

#endif
