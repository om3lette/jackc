#ifndef JACKC_COMPILER_FRONTEND_H
#define JACKC_COMPILER_FRONTEND_H

#include "compiler/ast/ast.h"
#include "core/allocators/allocators.h"

typedef struct {
    ast_class* ast;
    bool had_error;
} jackc_parse_result;

typedef struct jack_ast_collection {
    ast_class* ast;
    jackc_string source;
    struct jack_ast_collection* next;
} jack_ast_collection;

int jackc_frontend_compile(const char* base_path, Allocator* allocator);

#endif
