#ifndef JACKC_COMPILER_DIAGNOSTICS_ENGINE_ENGINE_H
#define JACKC_COMPILER_DIAGNOSTICS_ENGINE_ENGINE_H

#include "compiler/lexer/compiler_lexer.h"
#include <stddef.h>
#include <stdint.h>

#define MAX_DIAGNOSTICS 128

typedef enum {
    DIAG_SEV_ERROR,
    DIAG_SEV_WARNING
} jack_diagnostic_severity;

typedef enum {
    DIAG_UNEXPECTED_TOKEN,
    DIAG_MISSING_VARIABLE_KIND,
    DIAG_INVALID_VARIABLE_TYPE
} jack_diagnostic_code;

typedef struct {
    jack_location loc;
    jack_diagnostic_severity severity;
    jack_diagnostic_code code;

    union {
        struct {
            int32_t expected;
            int32_t got;
        } unexpected_token;
    } data;
} jack_diagnostic;


typedef struct {
    size_t size;
    bool overflow;

    jack_diagnostic diagnostics[MAX_DIAGNOSTICS];
} jack_diagnostic_engine;

#define jack_diag_engine_init() (jack_diagnostic_engine){ .size = 0, .overflow = false }

void jack_diagnostic_push(jack_diagnostic_engine* engine, jack_diagnostic diagnostic);

#endif
