#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include "frontend.h"

int main(int argc, char** argv) {
    if (argc != FRONTEND_EXPECTED_ARGUMENTS) {
        jackc_printf("Usage: jackc_frontend <base-dir> <save-path>\n");
        jackc_exit(FRONTEND_INVALID_ARGUMENT);
    }
    const char* base_dir_path = argv[FIRST_ARG_IDX];
    const char* out_dir_path = argv[FIRST_ARG_IDX + 1];

    Allocator allocator = arena_allocator_adapter();

    jackc_frontend_return_code result = jackc_frontend_compile(
        base_dir_path, out_dir_path, &allocator, false
    );

    arena_allocator_destroy(allocator.context);
    return (int)result;
}
