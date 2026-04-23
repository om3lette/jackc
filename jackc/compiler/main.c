#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/cli.h"
#include "core/localization/locale.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_stdio.h"
#include "std/jackc_string.h"
#include "frontend.h"
#include <stddef.h>

typedef struct {
    jackc_cli_args common;
    const char* lang;
} cmd_arguments;

static cmd_arguments cmd_args = {
    .common = COMMON_CLI_ARGS,
    .lang = nullptr
};
static arg_spec argument_specs[] = {
    arg_spec_create("-s", "--source-dir", CLI_SOURCE_DIR, ARG_STRING, offsetof(cmd_arguments, common) + offsetof(jackc_cli_args, source_dir), true),
    arg_spec_create("-o", "--out-dir", CLI_OUT_DIR, ARG_STRING, offsetof(cmd_arguments, common) + offsetof(jackc_cli_args, out_dir), true),
    arg_spec_create("-std", "--stdlib-dir", CLI_STD_DIR, ARG_STRING, offsetof(cmd_arguments, common) + offsetof(jackc_cli_args, stdlib_dir), true),
    arg_spec_create(nullptr, "--lang", CLI_LANGUAGE, ARG_STRING, offsetof(cmd_arguments, lang), false)
};

int main(int argc, char** argv) {
    Allocator allocator = arena_allocator_adapter();

    jackc_language_code lang_code = JACKC_DEFAULT_LOCALE;
    for (int i = 1; i < argc; ++i) {
        if (jackc_strcmp(argv[i], "--lang") == 0 && i + 1 < argc) {
            jackc_cli_parse_lang(argv[i + 1], &lang_code);
            break;
        }
    }
    const jackc_locale* locale = jackc_locale_get(lang_code);

    if (parse_args(&cmd_args, argument_specs, sizeof(argument_specs) / sizeof(arg_spec), argc, argv, locale, &allocator)) {
        jackc_exit(FRONTEND_INVALID_ARGUMENT);
    }

    const char* input_paths[] = { cmd_args.common.source_dir, cmd_args.common.stdlib_dir };

    jackc_frontend_config frontend_config = {
        .locale = locale,
        .diag_engine_output_fd = jackc_stdout_fd(),
        .diag_engine_filename_override = false,
        .skip_vm_code_gen = false
    };

    jackc_frontend_return_code return_code = jackc_frontend_compile(
        input_paths, 2, cmd_args.common.out_dir, &frontend_config, &allocator
    );

    if (return_code != FRONTEND_OK) {
        jackc_printf(locale->msgs.frontend_failed, return_code);
        jackc_putchar('\n');
    }
    arena_allocator_destroy(allocator.context);
    jackc_exit((int)return_code);
}
