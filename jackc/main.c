#include "compiler/frontend.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"
#include "core/exit_code.h"
#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include "vm-translator/backend.h"

int main(int argc, char** argv) {
    if (argc != JACKC_EXPECTED_ARGUMENTS) {
        jackc_printf("Usage: jackc <base-dir> <out-dir>\n");
        jackc_exit(JACKC_INVALID_NUMBER_OF_ARGUMENTS);
    }
    const char* base_dir_path = argv[FIRST_ARG_IDX];
    const char* out_dir_path = argv[FIRST_ARG_IDX + 1];

    Allocator arena = arena_allocator_adapter();
    jackc_frontend_return_code frontend_ret_code = jackc_frontend_compile(base_dir_path, out_dir_path, &arena, false);
    if (frontend_ret_code != FRONTEND_OK) {
        jackc_exit((int)frontend_ret_code);
    }
    // TODO: Unhardcode config values
    const jackc_config_t* jackc_config = jackc_config_create(false, 256 * 1024);
    jackc_backend_return_code backend_ret_code = jackc_backend_compile(out_dir_path, out_dir_path, jackc_config);

    arena_allocator_destroy(arena.context);
    jackc_exit((int)backend_ret_code);
}
