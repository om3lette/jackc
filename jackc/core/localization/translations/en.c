#include "core/localization/locale.h"
#include "compiler/diagnostics-engine/diagnostic.h"

static const jackc_diagnostic_translation diagnostic_translations_en[] = {
    { DIAG_UNEXPECTED_TOKEN, "Expected '%s', got '%.*s'", nullptr },
    { DIAG_MISSING_VARIABLE_KIND, "Missing variable kind", nullptr },
    { DIAG_MISSING_VARIABLE_NAME, "Missing variable name", "Perhaps you forgot to specify a type?" },
    { DIAG_INVALID_VARIABLE_TYPE, "Invalid variable type.", "Expected: 'int' | 'char' | 'boolean' | className" },
    { DIAG_INVALID_RETURN_TYPE, "Invalid return type.", "Expected: 'void' | 'int' | 'char' | 'boolean' | className" },
    { DIAG_INVALID_SUBROUTINE_KIND, "Invalid subroutine kind.", "Expected: 'function' | 'method' | 'constructor'" },
    { DIAG_INVALID_TOKEN_SUBROUTINE_BODY, "Invalid subroutine body: found '%.*s'.", "Expected a variable declaration or a statement."},
    { DIAG_INVALID_TOKEN_CLASS_BODY, "Invalid class member declaration: found '%.*s'.", "Expected a class variable or subroutine declaration." },
    { DIAG_INVALID_TOKEN_TERM, "Unexpected token '%.*s' while parsing term.", "Expected start of term: literal, identifier, '(', or unary operator." },
    { DIAG_MISSING_SEMICOLON, "Missing semicolon.", nullptr },
    { DIAG_MIXING_DECLARATIONS_AND_CODE, "Mixing declarations and code", nullptr },

    { DIAG_REDEFINITION, "Redefinition of '%.*s'.", nullptr },
    { DIAG_INCOMPLETE_TYPE, "Variable '%.*s' has incomplete type.", nullptr },
    { DIAG_USE_OF_UNDECLARED_IDENTIFIER, "Undeclared identifier '%.*s'.", nullptr },
    { DIAG_CALL_TO_UNDECLARED_SUBROUTINE, "Call to undeclared subroutine '%.*s'.", nullptr },
    { DIAG_NON_VOID_SUBROUTINE_SHOULD_RETURN_A_VALUE, "Non 'void' subroutine should return a value.", nullptr },
    { DIAG_CALLED_OBJECT_TYPE_IS_NOT_A_CLASS, "Called object is not of class type.", nullptr },
    { DIAG_TOO_FEW_ARGUMENTS_TO_FUNCTION_CALL, "Too few arguments to subroutine call, expected %d, have %d", nullptr },
    { DIAG_TOO_MANY_ARGUMENTS_TO_FUNCTION_CALL, "Too many arguments to subroutine call, expected %d, have %d", nullptr },
    { DIAG_INCOMPATIBLE_TYPE_CONVERSION, "Incompatible conversion from '%.*s' to '%.*s'", nullptr },

    { DIAG_EMPTY_IF_STATEMENT, "Empty if statement body", nullptr },
    { DIAG_INVALID_OPERATION, "Invalid operation.", nullptr },
    { DIAG_CANNOT_CALL_METHOD_WITHOUT_AN_OBJECT, "Cannot call method '%.*s' without an object", nullptr },
    { DIAG_CLASS_NAME_DOES_NOT_MATCH_THE_FILENAME, "The class name '%.*s' does not match the filename '%.*s'.", nullptr },

    { DIAG_NOTE_PREVIOUS_DEFINITION_IS_HERE, "Previous definition is here", nullptr },
    { DIAG_NOTE_DECLARED_HERE, "Declared here", nullptr },

    { DIAG_WARNING_CONSTRUCTOR_WITH_NO_DISPOSE, "No 'dispose' method for non static class", "Will cause memory leaks" },
};
static_assert(NUMBER_OF_DIAGNOSTICS == sizeof(diagnostic_translations_en) / sizeof(diagnostic_translations_en[0]), "Diagnostic translations array size mismatch");

const jackc_locale jackc_locale_en = {
    .cli = {
        .usage = "Usage: jackc [options]",
        .options = "Options (* - required):",
        .unknown_arg = "Unknown argument: %s",
        .required_arg = "Required argument '%s' was not provided",
        .option_descriptions = {
            {CLI_SOURCE_DIR, "Source files directory"},
            {CLI_OUT_DIR, "Output directory"},
            {CLI_STD_DIR, "Path to the stdlib directory"},
            {CLI_CODE_COMMENTS,  "Enables generation of code comments"},
            {CLI_LANGUAGE, "Language (en/ru)"},
            {CLI_REVERSED_STACK, "Stack will grow towards higher addresses"},
            {CLI_STACK_SIZE, "Fixed stack size (only works with --reversed-stack)"}
        }
    },
    .msgs = {
        .frontend_failed = "Frontend failed with exit code: %d",
        .backend_failed = "Backend failed with exit code: %d",
        .parser_failed_with_exit_code = "Parser failed with exit code %d",
        .program_entrypoint_not_found = "Main.main not found in symbol table"
    },
    .files = {
        .failed_open_base_dir = "Failed to open base directory %s",
        .failed_stat = "Failed to stat %s",
        .failed_close = "Failed to close file descriptor",
        .failed_read = "Failed to read file",
        .failed_rewind = "Failed to rewind file",
        .max_dir_depth = "Max directory depth reached",
        .failed_extract_filename = "Failed to extract filename from %s",
        .failed_open_file = "Failed to open file at %s"
    },
    .diagnostics = {
        .entries = diagnostic_translations_en,
        .count = NUMBER_OF_DIAGNOSTICS,

        .error = "Error",
        .warning = "Warning",
        .note = "Note",
        .engine_overflow = "Diagnostic engine overflowed"
    },
    .asm_code_gen = {
        .inline_call_to = "Inlined call to %.*s",
        .end_of_inlined_call = "End of inlined call",
        .save_registers = "Save registers",
        .restore_registers = "Restore registers",
        .set_arg_ptr = "Set ARG pointer",
        .push_ret_value = "Put return value on the stack",
        .pop_ret_value = "Pop return value from the stack",
        .restore_stack_ptr = "Restore stack pointer",
        .allocate_space_for_local_and_setup_local_ptr = "Allocate space for local variables and setup local ptr",
        .init_local_ptr = "Initialize local ptr as a frame anchor",
        .init_static_ptr = "Initialize STATIC pointer",
        .init_tmp_ptr = "Initialize TEMP pointer",
        .allocate_space_for_the_upward_growing_stack = "Allocate space for the stack (grows upwards)",
        .prepare_argc_argv = "Prepare argc, argv for Main.main",
    }
};
