#ifndef JACKC_COMPILER_AST_TRAVERSALS_H
#define JACKC_COMPILER_AST_TRAVERSALS_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/symtable/compiler_symtable.h"

typedef struct {
    function_registry* registry;
    jackc_diagnostic_engine engine;
    bool had_redeclaration;
} function_registry_traversal_context;

void ast_function_registry_build_traversal(const ast_class* class, function_registry_traversal_context* ctx);

typedef struct {
    jackc_string class_name;

    jackc_string subroutine_name;
    function_signature sub_signature;

    sym_table* symtab;
    const function_registry* registry;
    jackc_diagnostic_engine engine;

    Allocator* allocator;

    bool is_invalid;
} semantic_validity_traversal_context;

bool ast_semantic_validity_traversal(const ast_class* class, semantic_validity_traversal_context* ctx);

#endif
