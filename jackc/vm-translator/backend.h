#ifndef JACKC_BACKEND_H
#define JACKC_BACKEND_H

#include "common/config.h"
#include <stdint.h>

#define OUT_FILENAME "out.asm"

typedef enum {
    BACKEND_OK                          = 0,
    // 1-9 is reserved for shared exit codes
    BACKEND_INVALID_ARGUMENT            = 1000,
    BACKEND_INVALID_SYNTAX              = 1001,
    BACKEND_FAILED_TO_OPEN_SOURCE_FILE  = 1002,
    BACKEND_FAILED_TO_OPEN_SAVE_FILE    = 1003,
    BACKEND_NO_SOURCE_FILES             = 1004
} jackc_backend_return_code;

jackc_backend_return_code jackc_backend_compile(
    const char* base_path,
    const char* output_dir,
    const jackc_config_t* config
);

#endif
