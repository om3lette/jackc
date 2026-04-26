#ifndef JACKC_COMPILER_AST_TRAVERSALS_H
#define JACKC_COMPILER_AST_TRAVERSALS_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/symtable/compiler_symtable.h"
#include "core/allocators/allocators.h"
#include "std/jackc_syscalls.h"

typedef struct {
    function_registry* registry;
    jackc_diagnostic_engine engine;
    bool had_redeclaration;
} function_registry_traversal_context;

/**
 * Inserts all subroutines of the given class into the function registry.
 * Updates `had_redeclaration` in the context if a redeclaration is detected.
 * Will emit a diagnostic for each redeclaration.
 * 
 * @param class The class to build the function registry for.
 * @param ctx The function registry traversal context.
 */
void ast_function_registry_build_traversal(const ast_class* class, function_registry_traversal_context* ctx);

typedef struct {
    jackc_string class_name;
    jackc_span class_span;

    jackc_string subroutine_name;
    jackc_span subroutine_span;
    function_signature sub_signature;

    sym_table* symtab;
    const function_registry* registry;
    jackc_diagnostic_engine engine;

    Allocator* allocator;

    bool is_invalid;
    bool has_constructor;
    bool has_dispose_method;
    bool has_return_stmt;
} semantic_validity_traversal_context;

[[ nodiscard ]] semantic_validity_traversal_context semantic_validity_traversal_context_init(
    const function_registry* registry,
    jackc_diagnostic_engine* engine,
    Allocator* allocator
);

/**
 * Traverses the AST and performs semantic validation.
 * Updates `is_invalid` in the context if a semantic error is detected.
 * Will emit a diagnostic for each semantic error.
 * 
 * @param class The class to validate.
 * @param ctx The semantic validity traversal context.
 * @return `true` if the class is valid, `false` otherwise.
 */
[[ nodiscard ]] bool ast_semantic_validity_traversal(const ast_class* class, semantic_validity_traversal_context* ctx);

/**
 * Enters a class scope, updating the context with the class's fields and subroutines.
 * 
 * @param ctx The semantic validity traversal context.
 * @param class The class to enter.
 */
void semantic_validity_enter_class(semantic_validity_traversal_context* ctx, const ast_class* class);

/**
 * Enters a subroutine scope, updating the context with the subroutine's parameters and locals.
 * 
 * @param ctx The semantic validity traversal context.
 * @param sub The subroutine to enter.
 */
void semantic_validity_enter_subroutine(semantic_validity_traversal_context* ctx, const ast_subroutine* sub);

typedef struct {
    FD fd;
    bool had_error;

    sym_table* symtab;
    const function_registry* registry;
    Allocator* allocator;

    jackc_string class_name;
    uint16_t n_fields;
    function_signature subroutine_signature;

    ast_var_dec* this;

    uint32_t if_label_index;
    uint32_t while_label_index;
} vm_code_generation_traversal_context;

/**
 * Traverses the AST and generates VM code.
 * Expects the input to be semantically valid.
 * Semantic errors will not produce diagnostics, but `had_error` flag will be set.
 * 
 * @param class The class to generate VM code for.
 * @param ctx The VM code generation traversal context.
 */
void vm_code_genetation_traversal(const ast_class* class, vm_code_generation_traversal_context* ctx);

#endif
