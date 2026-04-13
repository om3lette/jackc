#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/cli.h"
#include "std/jackc_stdlib.h"
#include "frontend.h"

typedef struct {
    const char* source_dir;
    const char* out_dir;
    const char* stdlib_dir;
} cmd_arguments;

static cmd_arguments cmd_args = {0};
static arg_spec argument_specs[] = {
    arg_spec_create("-s", "--source-dir", "Source files directory", ARG_STRING, offsetof(cmd_arguments, source_dir), true),
    arg_spec_create("-o", "--out-dir", "Output directory", ARG_STRING, offsetof(cmd_arguments, out_dir), true),
    arg_spec_create("-std", "--stdlib-dir", "Path to the stdlib directory", ARG_STRING, offsetof(cmd_arguments, stdlib_dir), true),
};

int main(int argc, char** argv) {
    Allocator allocator = arena_allocator_adapter();
    if (parse_args(&cmd_args, argument_specs, sizeof(argument_specs) / sizeof(arg_spec), argc, argv, &allocator)) {
        jackc_exit(FRONTEND_INVALID_ARGUMENT);
    }

    const char* input_paths[] = { cmd_args.source_dir, cmd_args.stdlib_dir };
    jackc_frontend_return_code return_code = jackc_frontend_compile(
        input_paths, 2, cmd_args.out_dir, &allocator, false
    );

    if (return_code != FRONTEND_OK) {
        jackc_printf("Frontend failed with exit code: %d\n", return_code);
    }
    arena_allocator_destroy(allocator.context);
    jackc_exit((int)return_code);
}
