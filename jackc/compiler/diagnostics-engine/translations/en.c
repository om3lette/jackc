#include "compiler/diagnostics-engine/diagnostic.h"
#include "translation.h"
#include <assert.h>

const jackc_diagnostic_translation diagnostic_translations[] = {
    { DIAG_UNEXPECTED_TOKEN, "Expected '%s', got '%.*s'", nullptr },
    { DIAG_MISSING_VARIABLE_KIND, "Missing variable kind", nullptr },
    { DIAG_MISSING_VARIABLE_NAME, "Missing variable name", "Perhaps you forgot to specify a type?" },
    { DIAG_INVALID_VARIABLE_TYPE, "Invalid variable type.", "Expected: 'int' | 'char' | 'boolean' | className" },
    { DIAG_INVALID_RETURN_TYPE, "Invalid return type.", "Expected: 'void' | 'int' | 'char' | 'boolean' | className" },
    { DIAG_INVALID_SUBROUTINE_KIND, "Invalid subroutine kind.", "Expected: 'function' | 'method' | 'constructor'" },
    { DIAG_INVALID_TOKEN_CLASS_BODY, "Invalid class member declaration: found '%.*s'.", "Expected a class variable or subroutine declaration." },
    { DIAG_INVALID_TOKEN_TERM, "Unexpected token '%.*s' while parsing term.", "Expected start of term: literal, identifier, '(', or unary operator." },
    { DIAG_MISSING_SEMICOLON, "Missing semicolon.", nullptr },
};

static_assert(NUMBER_OF_DIAGNOSTICS == sizeof(diagnostic_translations) / sizeof(diagnostic_translations[0]), "Diagnostic translations array size mismatch");
