#include "translation.h"

const jackc_diagnostic_translation diagnostic_translations[] = {
    { DIAG_UNEXPECTED_TOKEN, "Unexpected token" },
    { DIAG_MISSING_VARIABLE_KIND, "Missing variable kind" },
    { DIAG_MISSING_VARIABLE_NAME, "Missing variable name" },
    { DIAG_INVALID_VARIABLE_TYPE, "Invalid variable type (int | char | boolean | <className>)" },
    { DIAG_INVALID_RETURN_TYPE, "Invalid return type (void | int | char | boolean | <className>)" }
};
