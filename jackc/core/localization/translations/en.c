#include "core/localization/locale.h"
#include "compiler/diagnostics-engine/diagnostic.h"

static const jackc_diagnostic_translation diagnostic_translations_en[] = {
    { "DIAG_UNEXPECTED_TOKEN", "Expected '%s', got '%.*s'", nullptr },
    { "DIAG_MISSING_VARIABLE_KIND", "Missing variable kind", nullptr },
    { "DIAG_MISSING_VARIABLE_NAME", "Missing variable name", "Perhaps you forgot to specify a type?" },
    { "DIAG_INVALID_VARIABLE_TYPE", "Invalid variable type.", "Expected: 'int' | 'char' | 'boolean' | className" },
    { "DIAG_INVALID_RETURN_TYPE", "Invalid return type.", "Expected: 'void' | 'int' | 'char' | 'boolean' | className" },
    { "DIAG_INVALID_SUBROUTINE_KIND", "Invalid subroutine kind.", "Expected: 'function' | 'method' | 'constructor'" },
    { "DIAG_INVALID_TOKEN_SUBROUTINE_BODY", "Invalid subroutine body: found '%.*s'.", "Expected a variable declaration or a statement."},
    { "DIAG_INVALID_TOKEN_CLASS_BODY", "Invalid class member declaration: found '%.*s'.", "Expected a class variable or subroutine declaration." },
    { "DIAG_INVALID_TOKEN_TERM", "Unexpected token '%.*s' while parsing term.", "Expected start of term: literal, identifier, '(', or unary operator." },
    { "DIAG_MISSING_SEMICOLON", "Missing semicolon.", nullptr },
    { "DIAG_MIXING_DECLARATIONS_AND_CODE", "Mixing declarations and code", nullptr },

    { "DIAG_REDEFINITION", "Redefinition of '%.*s'.", nullptr },
    { "DIAG_INCOMPLETE_TYPE", "Variable '%.*s' has incomplete type.", nullptr },
    { "DIAG_USE_OF_UNDECLARED_IDENTIFIER", "Undeclared identifier '%.*s'.", nullptr },
    { "DIAG_CALL_TO_UNDECLARED_SUBROUTINE", "Call to undeclared subroutine '%.*s'.", nullptr },
    { "DIAG_NON_VOID_SUBROUTINE_SHOULD_RETURN_A_VALUE", "Non 'void' subroutine should return a value.", nullptr },
    { "DIAG_CALLED_OBJECT_TYPE_IS_NOT_A_CLASS", "Called object is not of class type.", nullptr },
    { "DIAG_TOO_FEW_ARGUMENTS_TO_FUNCTION_CALL", "Too few arguments to subroutine call, expected %d, have %d", nullptr },
    { "DIAG_TOO_MANY_ARGUMENTS_TO_FUNCTION_CALL", "Too many arguments to subroutine call, expected %d, have %d", nullptr },

    { "DIAG_EMPTY_IF_STATEMENT", "Empty if statement body", nullptr },
    { "DIAG_INVALID_OPERATION", "Invalid operation.", nullptr },
    { "DIAG_CANNOT_CALL_METHOD_WITHOUT_AN_OBJECT", "Cannot call method '%.*s' without an object", nullptr },
    { "DIAG_CLASS_NAME_DOES_NOT_MATCH_THE_FILENAME", "The class name '%.*s' does not match the filename '%.*s'.", nullptr },

    { "DIAG_NOTE_PREVIOUS_DEFINITION_IS_HERE", "Previous definition is here", nullptr },
    { "DIAG_NOTE_DECLATED_HERE", "Declared here", nullptr },

    { "DIAG_WARNING_CONSTRUCTOR_WITH_NO_DISPOSE", "No 'dispose' method for non static class", "Will cause memory leaks" },
};
static_assert(NUMBER_OF_DIAGNOSTICS == sizeof(diagnostic_translations_en) / sizeof(diagnostic_translations_en[0]), "Diagnostic translations array size mismatch");

const jackc_locale jackc_locale_en = {
    .cli = {
        .usage = "Usage: jackc [options]",
        .options = "Options (* - required):",
        .unknown_arg = "Unknown argument: %s",
        .required_arg = "Required argument '%s' was not provided"
    },
    .diagnostics = {
        .entries = diagnostic_translations_en,
        .count = NUMBER_OF_DIAGNOSTICS
    }
};
