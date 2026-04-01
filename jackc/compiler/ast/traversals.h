#ifndef JACKC_COMPILER_AST_TRAVERSALS_H
#define JACKC_COMPILER_AST_TRAVERSALS_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/symtable/compiler_symtable.h"
#include "core/allocators/allocators.h"

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
    bool has_constructor;
    bool has_dispose_method;
} semantic_validity_traversal_context;

semantic_validity_traversal_context semantic_validity_traversal_context_init(
    const jackc_string* class_name,
    const function_registry* registry,
    jackc_diagnostic_engine* engine,
    Allocator* allocator
);

bool ast_semantic_validity_traversal(const ast_class* class, semantic_validity_traversal_context* ctx);

typedef struct {
    int fd;
    bool had_error;

    sym_table* symtab;
    const function_registry* registry;
    Allocator* allocator;

    jackc_string class_name;
    uint16_t n_fields;
    function_signature subroutine_signature;

    uint32_t if_label_index;
    uint32_t while_label_index;
} vm_code_generation_traversal_context;

void vm_code_genetation_traversal(const ast_class* class, vm_code_generation_traversal_context* ctx);

#endif
