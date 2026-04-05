#include "compiler/frontend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/cli.h"
#include "core/exit_code.h"
#include "std/jackc_stdlib.h"
#include "vm-translator/backend.h"
#include <stddef.h>

typedef struct {
    const char* source_dir;
    const char* out_dir;
    const char* stdlib_dir;
    bool reversed_stack;
    uint32_t stack_size;
} cmd_arguments;

static cmd_arguments cmd_args = {
    .reversed_stack = false,
    .stack_size = 256 * 1024
};
static arg_spec argument_specs[] = {
    arg_spec_create("-s", "--source-dir", "Source files directory", ARG_STRING, offsetof(cmd_arguments, source_dir), true),
    arg_spec_create("-o", "--out-dir", "Output directory", ARG_STRING, offsetof(cmd_arguments, out_dir), true),
    arg_spec_create("-std", "--stdlib-dir", "Path to the stdlib directory", ARG_STRING, offsetof(cmd_arguments, stdlib_dir), true),
    arg_spec_create(nullptr, "--reversed-stack", "Stack will grow towards higher addresses", ARG_BOOL, offsetof(cmd_arguments, reversed_stack), false),
    arg_spec_create(nullptr, "--stack-size", "Fixed stack size (only works with --reversed-stack)", ARG_UINT, offsetof(cmd_arguments, stack_size), false),
};

int main(int argc, char** argv) {
    Allocator arena = arena_allocator_adapter();
    if (parse_args(&cmd_args, argument_specs, sizeof(argument_specs) / sizeof(arg_spec), argc, argv, &arena)) {
        jackc_exit(JACKC_INVALID_NUMBER_OF_ARGUMENTS);
    }

    const char* input_paths[] = { cmd_args.stdlib_dir, cmd_args.source_dir };
    jackc_frontend_return_code frontend_ret_code = jackc_frontend_compile(
        input_paths, 2, cmd_args.out_dir, &arena, false
    );

    if (frontend_ret_code != FRONTEND_OK) {
        jackc_printf("Frontend failed with exit code: %d\n", frontend_ret_code);
        jackc_exit((int)frontend_ret_code);
    }

    jackc_config config = jackc_config_create(cmd_args.reversed_stack, cmd_args.stack_size);
    jackc_backend_return_code backend_ret_code = jackc_backend_compile(cmd_args.out_dir, cmd_args.out_dir, &config);

    arena_allocator_destroy(arena.context);
    if (backend_ret_code != BACKEND_OK) {
        jackc_printf("Backend failed with exit code: %d\n", frontend_ret_code);
    }
    jackc_exit((int)backend_ret_code);
}
