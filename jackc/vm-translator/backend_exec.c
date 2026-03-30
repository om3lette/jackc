#include "common/config.h"
#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include "vm-translator/backend.h"

int main(int argc, char** argv) {
    if (argc != BACKEND_EXPECTED_ARGUMENTS) {
        jackc_printf("Usage: jackc <base-dir> <save-path>\n");
        jackc_exit(BACKEND_INVALID_ARGUMENT);
    }
    const char* base_dir_path = argv[FIRST_ARG_IDX];
    const char* out_dir_path = argv[FIRST_ARG_IDX + 1];

    // TODO: Unhardcode config variables
    const jackc_config_t* const jackc_config = jackc_config_create(false, 256 * 1024);
    return (int)jackc_backend_compile(base_dir_path, out_dir_path, jackc_config);
}
