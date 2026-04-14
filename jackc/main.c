#include "compiler/frontend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/cli.h"
#include "core/exit_code.h"
#include "core/localization/locale.h"
#include "std/jackc_stdlib.h"
#include "vm-translator/backend.h"
#include <stddef.h>

typedef struct {
    jackc_cli_args common;
    bool reversed_stack;
    bool code_comments;
    uint32_t stack_size;
    const char* lang;
} cmd_arguments;

static cmd_arguments cmd_args = {
    .common = COMMON_CLI_ARGS,
    .reversed_stack = false,
    .stack_size = 256 * 1024,
    .code_comments = false,
    .lang = nullptr
};
static arg_spec argument_specs[] = {
    arg_spec_create("-s", "--source-dir", "Source files directory", ARG_STRING, offsetof(cmd_arguments, common) + offsetof(jackc_cli_args, source_dir), true),
    arg_spec_create("-o", "--out-dir", "Output directory", ARG_STRING, offsetof(cmd_arguments, common) + offsetof(jackc_cli_args, out_dir), true),
    arg_spec_create("-std", "--stdlib-dir", "Path to the stdlib directory", ARG_STRING, offsetof(cmd_arguments, common) + offsetof(jackc_cli_args, stdlib_dir), true),
    // arg_spec_create(nullptr, "--reversed-stack", "Stack will grow towards higher addresses", ARG_BOOL, offsetof(cmd_arguments, reversed_stack), false),
    // arg_spec_create(nullptr, "--stack-size", "Fixed stack size (only works with --reversed-stack)", ARG_UINT, offsetof(cmd_arguments, stack_size), false),
    arg_spec_create(nullptr, "--code-comments", "Enables generation of code comments", ARG_BOOL, offsetof(cmd_arguments, code_comments), false),
    arg_spec_create(nullptr, "--lang", "Language (en/ru)", ARG_STRING, offsetof(cmd_arguments, lang), false)
};

int main(int argc, char** argv) {
    Allocator allocator = arena_allocator_adapter();
    if (parse_args(&cmd_args, argument_specs, sizeof(argument_specs) / sizeof(arg_spec), argc, argv, &allocator)) {
        jackc_exit(JACKC_INVALID_NUMBER_OF_ARGUMENTS);
    }

    jackc_language_code lang_code;
    if (!jackc_cli_parse_lang(cmd_args.lang, &lang_code)) {
        jackc_printf("Invalid language specified, using default (%s)\n", jackc_lang_to_readable(lang_code));
        lang_code = JACKC_DEFAULT_LOCALE;
    }
    const jackc_locale* locale = jackc_locale_get(lang_code);

    const char* input_paths[] = { cmd_args.common.stdlib_dir, cmd_args.common.source_dir };
    jackc_frontend_return_code frontend_ret_code = jackc_frontend_compile(
        input_paths, 2, cmd_args.common.out_dir, locale, &allocator, false
    );

    if (frontend_ret_code != FRONTEND_OK) {
        jackc_printf("Frontend failed with exit code: %d\n", frontend_ret_code);
        jackc_exit((int)frontend_ret_code);
    }

    jackc_config config = jackc_config_create(cmd_args.reversed_stack, cmd_args.stack_size, cmd_args.code_comments);
    jackc_backend_return_code backend_ret_code = jackc_backend_compile(
        cmd_args.common.out_dir, cmd_args.common.out_dir, cmd_args.common.stdlib_dir, &config, &allocator
    );

    arena_allocator_destroy(allocator.context);
    if (backend_ret_code != BACKEND_OK) {
        jackc_printf("Backend failed with exit code: %d\n", backend_ret_code);
    }
    jackc_exit((int)backend_ret_code);
}
