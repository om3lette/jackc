#ifndef JACKC_COMPILER_FRONTEND_H
#define JACKC_COMPILER_FRONTEND_H

#include "compiler/ast/ast.h"
#include "core/allocators/allocators.h"
#include <stdint.h>

typedef enum {
    FRONTEND_OK = 0,
    // 1-9 is reserved for shared exit codes
    FRONTEND_INVALID_ARGUMENT           = 10,
    FRONTEND_SYNTAX_ERROR               = 11,
    FRONTEND_FAILED_TO_OPEN_SOURCE_FILE = 12,
    FRONTEND_SYMBOL_TABLE_BUILD_ERROR   = 13,
    FRONTEND_NO_SOURCE_FILES            = 14,
    FRONTEND_SEMANTICALLY_INVALID       = 15,
    FRONTEND_FAILED_TO_GENERATE_VM_CODE = 16
} jackc_frontend_return_code;

typedef struct {
    ast_class* ast;
    uint32_t lines;
    bool had_error;
} jackc_parse_result;

typedef struct jack_ast_collection {
    ast_class* ast;
    const char* filepath;
    uint32_t lines;
    jackc_string content;
    struct jack_ast_collection* next;
} jack_source;

jackc_frontend_return_code jackc_frontend_compile(
    const char* base_path,
    const char* output_dir,
    Allocator* allocator,
    bool skip_vm_code_gen
);

#endif
