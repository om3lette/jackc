#ifndef JACKC_COMPILER_DIAGNOSTICS_ENGINE_DIAGNOSTIC_H
#define JACKC_COMPILER_DIAGNOSTICS_ENGINE_DIAGNOSTIC_H

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
    DIAG_UNEXPECTED_TOKEN,
    DIAG_MISSING_VARIABLE_KIND,
    DIAG_MISSING_VARIABLE_NAME,
    DIAG_INVALID_VARIABLE_TYPE,
    DIAG_INVALID_RETURN_TYPE
} jackc_diagnostic_code;

typedef struct {
    jackc_span span;
    jackc_diagnostic_severity severity;
    jackc_diagnostic_code code;

    union {
        struct {
            int32_t expected;
            int32_t got;
        } unexpected_token;
    } data;
} jackc_diagnostic;

#endif
