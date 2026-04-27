#ifndef AST_TRAVERSAL_UTILS_H
#define AST_TRAVERSAL_UTILS_H

#include "compiler/ast/traversals.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/symtable/symtable_token.h"
#include "std/jackc_string.h"

typedef struct {
    jackc_string receiver_class;
    jackc_string receiver;
    sym_table_token var;
    function_signature signature;
    bool is_function_call;
    bool is_method_call;
    bool is_var_in_symtab;
    bool success;
} jack_call_resolution;

[[ nodiscard ]] jack_call_resolution resolve_subroutine_call_receiver(
    const ast_call* call,
    const jackc_string* current_class,
    const sym_table* symtab,
    const function_registry* registry
);

/**
 * Calculates the final type of an expression.
 * Will return `TYPE_VOID` if the type cannot be determined.
 * 
 * @param ctx The semantic validity traversal context.
 * @param expr The expression.
 * @return The resolved type of the expression.
 */
[[ nodiscard ]] ast_type resolve_expression_type(const semantic_validity_traversal_context* ctx, const ast_expr* expr);

[[ nodiscard ]] ast_type sym_table_token_to_ast_type(const sym_table_token* token);

/**
 * Checks if the given type is a primitive type.
 * Primitive types are: int, boolean, char.
 * 
 * @param kind The type kind.
 * @return True if the type is primitive, false otherwise.
 */
[[ nodiscard ]] bool is_primitive_type(ast_type_kind kind);

/**
 * Checks if right type can be converted to left type.
 * 
 * @param left The left type.
 * @param right The right type.
 * @return True if the types are compatible, false otherwise.
 */
[[ nodiscard ]] bool are_types_compatible(const ast_type* left, const ast_type* right);
#endif
