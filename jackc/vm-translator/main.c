#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/cli.h"
#include "core/config.h"
#include "core/localization/locale.h"
#include "std/jackc_stdlib.h"
#include "vm-translator/backend.h"
#include <stddef.h>

typedef struct {
    const char* source_dir;
    const char* out_dir;
    const char* stdlib_dir;
    bool reversed_stack;
    bool code_comments;
    uint32_t stack_size;
} cmd_arguments;

static cmd_arguments cmd_args = {
    .stdlib_dir = nullptr,
    .reversed_stack = false,
    .stack_size = 256 * 1024,
    .code_comments = false
};
static arg_spec argument_specs[] = {
    arg_spec_create("-s", "--source-dir", "Source files directory", ARG_STRING, offsetof(cmd_arguments, source_dir), true),
    arg_spec_create("-o", "--out-dir", "Output directory", ARG_STRING, offsetof(cmd_arguments, out_dir), true),
    arg_spec_create("-std", "--stdlib-dir", "Path to the stdlib directory", ARG_STRING, offsetof(cmd_arguments, stdlib_dir), true),
    // arg_spec_create(nullptr, "--reversed-stack", "Stack will grow towards higher addresses", ARG_BOOL, offsetof(cmd_arguments, reversed_stack), false),
    // arg_spec_create(nullptr, "--stack-size", "Fixed stack size (only works with --reversed-stack)", ARG_UINT, offsetof(cmd_arguments, stack_size), false),
    arg_spec_create(nullptr, "--code-comments", "Enables generation of code comments", ARG_BOOL, offsetof(cmd_arguments, code_comments), false),
};

int main(int argc, char** argv) {
    Allocator allocator = arena_allocator_adapter();
    if (parse_args(&cmd_args, argument_specs, sizeof(argument_specs) / sizeof(arg_spec), argc, argv, &allocator)) {
        jackc_exit(BACKEND_INVALID_ARGUMENT);
    }

    // TODO
    const jackc_locale* locale = jackc_locale_get(JACKC_DEFAULT_LOCALE);
    (void)locale;
    jackc_config config = jackc_config_create(cmd_args.reversed_stack, cmd_args.stack_size, cmd_args.code_comments);

    jackc_backend_return_code return_code = jackc_backend_compile(
        cmd_args.source_dir, cmd_args.out_dir, cmd_args.stdlib_dir, &config, &allocator
    );
    if (return_code != BACKEND_OK) {
        jackc_printf("Backend failed with exit code: %d\n", return_code);
    }
    jackc_exit((int)return_code);
}
