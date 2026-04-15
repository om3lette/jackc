#include "ast_traversal_utils.h"
#include "compiler/ast/ast.h"
#include "compiler/ast/traversals.h"

jack_call_resolution resolve_subroutine_call_receiver(
    const ast_call* call,
    const jackc_string* current_class,
    const sym_table* symtab,
    const function_registry* registry
) {
    bool had_error = false;
    bool is_var_in_symtab = false;

    jackc_string resolved_receiver_class = call->receiver;
    jackc_string resolved_receiver = resolved_receiver_class;

    // Resolve receiver
    sym_table_token receiver_var = {0};
    if (call->implicit_this_receiver) {
        resolved_receiver_class = *current_class;
        resolved_receiver = resolved_receiver_class;
    } else if (sym_table_find(symtab, &resolved_receiver, &receiver_var)){
        is_var_in_symtab = true;
        // receiver is a variable of class type -> resolve to the class name
        jackc_assert(receiver_var.type == JACK_CLASS);
        resolved_receiver_class = receiver_var.class_name;
    }

    function_signature signature;
    if (!function_registry_contains(registry, &resolved_receiver_class, &call->subroutine_name, &signature))
        had_error = true;

    return (jack_call_resolution) {
        .receiver_class = resolved_receiver_class,
        .receiver = resolved_receiver,
        .var = receiver_var,
        .signature = signature,
        .is_function_call = signature.kind == SUB_FUNCTION,
        .is_method_call = signature.kind == SUB_METHOD,
        .is_var_in_symtab = is_var_in_symtab,
        .success = !had_error
    };
}


ast_type sym_table_token_to_ast_type(const sym_table_token* token) {
    switch (token->type) {
        case JACK_CLASS: return (ast_type) { .kind = TYPE_CLASS, .class_name = token->class_name };
        case JACK_BOOLEAN:
        case JACK_CHAR:
        case JACK_INT: return (ast_type) { .kind = TYPE_INT };
    }

    jackc_assert(false);
    return (ast_type){0};
}

ast_type resolve_expression_type(const semantic_validity_traversal_context* ctx, const ast_expr* expr) {
    switch (expr->kind) {
        case EXPR_UNARY:
            return resolve_expression_type(ctx, expr->unary.operand);

        case EXPR_BINARY: {
            // Jack is loosely typed - allow any expression combination
            switch (expr->binary.op) {
                case BINARY_OP_ADD:
                case BINARY_OP_SUB:
                case BINARY_OP_MUL:
                case BINARY_OP_DIV:
                    return (ast_type) { .kind = TYPE_INT };
                case BINARY_OP_EQ:
                case BINARY_OP_LT:
                case BINARY_OP_GT:
                case BINARY_OP_AND:
                case BINARY_OP_OR:
                    return (ast_type) { .kind = TYPE_BOOLEAN };
            }
            return (ast_type) { .kind = TYPE_INT };
        }

        case EXPR_STRING:
            return (ast_type){ .kind = TYPE_CLASS, .class_name = jackc_string_from_str("String") };

        case EXPR_KEYWORD: {
            switch (expr->keyword_val) {
                case KEYWORD_TRUE:
                case KEYWORD_FALSE:
                case KEYWORD_NULL:
                    return (ast_type){ .kind = TYPE_INT };
                case KEYWORD_THIS:
                    return (ast_type){ .kind = TYPE_CLASS, .class_name = ctx->class_name };
            }
            break;
        }
        case EXPR_ARRAY_ACCESS:
        case EXPR_INT:
            return (ast_type){ .kind = TYPE_INT };

        case EXPR_CALL: {
            jack_call_resolution resolved_call = resolve_subroutine_call_receiver(&expr->call, &ctx->class_name, ctx->symtab, ctx->registry);
            if (!resolved_call.success)
                return (ast_type) {.kind = TYPE_VOID};
            return *resolved_call.signature.return_type;
        }

        case EXPR_VAR: {
            sym_table_token token;
            if (!sym_table_find(ctx->symtab, &expr->var_name, &token))
                return (ast_type) { .kind = TYPE_VOID };

            return sym_table_token_to_ast_type(&token);
        }
    }
    return (ast_type) { .kind = TYPE_INT };
}


bool is_primitive_type(ast_type_kind kind) {
    return kind == TYPE_INT || kind == TYPE_CHAR || kind == TYPE_BOOLEAN;
}

bool are_types_compatible(const ast_type* left, const ast_type* right) {
    if (left->kind == TYPE_VOID || right->kind == TYPE_VOID)
        return left->kind == right->kind;

    if (left->kind == TYPE_CLASS || right->kind == TYPE_CLASS) {
        // Only compare class names if BOTH are classes as class_name is undefined otherwise
        if (left->kind == TYPE_CLASS && right->kind == TYPE_CLASS) {
            if (jackc_string_cmp(&left->class_name, &right->class_name) == 0)
                return true;
        }

        // Arrays are loosely typed...
        if (left->kind == TYPE_CLASS && jackc_streq(&left->class_name, "Array"))
            return true;
        // Allow treating primitives as class pointers
        return is_primitive_type(left->kind) || is_primitive_type(right->kind);
    }

    // All primitives (int, char, boolean) are compatible with each other
    return is_primitive_type(left->kind) && is_primitive_type(right->kind);
}
