#include "core/config.h"
#include "std/jackc_stdio.h"
#include "std/jackc_stdlib.h"
#include "vm-translator/backend.h"

int main(int argc, char** argv) {
    if (argc != BACKEND_EXPECTED_ARGUMENTS) {
        jackc_printf("Usage: jackc_backend <base-dir> <save-path>\n");
        jackc_exit(BACKEND_INVALID_ARGUMENT);
    }
    const char* base_dir_path = argv[FIRST_ARG_IDX];
    const char* out_dir_path = argv[FIRST_ARG_IDX + 1];

    // TODO: Unhardcode config variables
    jackc_config config = jackc_config_create(false, 256 * 1024);

    jackc_backend_return_code return_code = jackc_backend_compile(base_dir_path, out_dir_path, &config);
    if (return_code != BACKEND_OK) {
        jackc_printf("Backend failed with exit code: %d\n", return_code);
    }
    jackc_exit((int)return_code);
}
