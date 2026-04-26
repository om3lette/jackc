#include "compiler/ast/ast.h"
#include "compiler/ast/traversals.h"
#include "compiler/ast/traversals/ast_traversal_utils.h"
#include "compiler/ast/traversals/code-gen/std_utils.h"
#include "compiler/ast/traversals/code-gen/code_gen_utils.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/symtable/compiler_symtable.h"
#include "compiler/symtable/symtable_token.h"
#include "core/asserts/jackc_assert.h"
#include "core/logging/logger.h"
#include "std/jackc_string.h"
#include "vm-translator/code-gen/regs.h"
#include "vm-translator/parser/vm_parser.h"
#include <stdint.h>

static jack_type ast_type_to_jack_type(const ast_type* type) {
    switch (type->kind) {
        case TYPE_INT:
            return JACK_INT;
        case TYPE_CHAR:
            return JACK_CHAR;
        case TYPE_BOOLEAN:
            return JACK_BOOLEAN;
        case TYPE_CLASS:
            return JACK_CLASS;
            break;
        case TYPE_VOID:
            jackc_assert(false && "Semantically invalid");
    }
    // Make the compiler happy
    return JACK_CLASS;
}

static vm_segment jack_var_kind_to_vm_segment(jack_variable_kind var_kind) {
    switch (var_kind) {
        case VAR_STATIC_: return SEGMENT_STATIC;
        case VAR_ARG:    return SEGMENT_ARG;
        case VAR_LOCAL:  return SEGMENT_LOCAL;
        case VAR_FIELD:  return SEGMENT_THIS;
    }
    // Make the compiler happy
    return SEGMENT_LOCAL;
}

static void register_var(sym_table* symtab, const ast_var_dec* var_dec) {
    sym_table_token token = sym_table_token_init(
        ast_type_to_jack_type(&var_dec->type),
        var_dec->kind,
        &var_dec->name,
        var_dec->type.kind == TYPE_CLASS ? &var_dec->type.class_name : nullptr
    );
    jackc_assert(sym_table_insert(symtab, &token) == SYMTAB_OK);
}

// Forward declaration for `visit_subroutine_call`
static void visit_expression(vm_code_generation_traversal_context* ctx, const ast_expr* expr);

static void visit_subroutine_call(vm_code_generation_traversal_context* ctx, const ast_call* call) {
    uint16_t n_args = 0;
    const ast_expr_list* last_arg = call->args;
    for (last_arg = call->args; last_arg && last_arg->next; last_arg = last_arg->next) {}

    for (const ast_expr_list* arg = last_arg; arg; arg = arg->prev) {
        visit_expression(ctx, arg->expr);
        ++n_args;
    }

    jack_call_resolution resolved_call = resolve_subroutine_call_receiver(
        call, &ctx->class_name, ctx->symtab, ctx->registry
    );
    if (!resolved_call.success) {
        ctx->had_error = true;
        return;
    }
    // Implicit this
    if (resolved_call.is_method_call)
        ++n_args;

    // Push 'this' for methods
    if (resolved_call.is_method_call) {
        if (call->implicit_this_receiver) {
            vm_emit_push(ctx->fd, SEGMENT_POINTER, POINTER_THIS);
        } else if (resolved_call.is_var_in_symtab) {
            vm_emit_push(ctx->fd, jack_var_kind_to_vm_segment(resolved_call.var.var.kind), resolved_call.var.var.idx);
        } else {
            LOG_FATAL("%.*s\n", resolved_call.receiver_class.length, resolved_call.receiver_class.data);
            ctx->had_error = true;
            return;
        }
    }

    vm_emit_call(ctx->fd, &resolved_call.receiver_class, &call->subroutine_name, n_args);
}

static void visit_expression(vm_code_generation_traversal_context* ctx, const ast_expr* expr) {
    switch (expr->kind) {
        case EXPR_INT:
            vm_emit_signed_const(ctx->fd, expr->int_val);
            break;
        case EXPR_STRING:
            vm_emit_string_from_string_literal(ctx->fd, &expr->string_val);
            break;
        case EXPR_KEYWORD:
            switch (expr->keyword_val) {
                case KEYWORD_TRUE:
                    vm_emit_signed_const(ctx->fd, 1);
                    break;
                case KEYWORD_FALSE:
                case KEYWORD_NULL:
                    vm_emit_signed_const(ctx->fd, 0);
                    break;
                case KEYWORD_THIS:
                    vm_emit_push(ctx->fd, SEGMENT_POINTER, 0);
                    break;
            }
            break;
        case EXPR_VAR: {
            sym_table_token token;
            if (!sym_table_find(ctx->symtab, &expr->var_name, &token)) {
                ctx->had_error = true;
                return;
            }
            vm_emit_push(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);
            break;
        }
        case EXPR_ARRAY_ACCESS: {
            sym_table_token token;
            if (!sym_table_find(ctx->symtab, &expr->array_access.var_name, &token)) {
                ctx->had_error = true;
                return;
            }
            vm_emit_push(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);
            visit_expression(ctx, expr->array_access.index);
            vm_emit_binary_arithmetic_op(ctx->fd, BINARY_OP_ADD);

            vm_emit_pop(ctx->fd, SEGMENT_POINTER, POINTER_THAT);
            vm_emit_push(ctx->fd, SEGMENT_THAT, 0);
            break;
        }
        case EXPR_CALL: {
            visit_subroutine_call(ctx, &expr->call);
            break;
        }
        case EXPR_UNARY:
        visit_expression(ctx, expr->unary.operand);
            vm_emit_unary_arithmetic(ctx->fd, expr->unary.op);
            break;
        case EXPR_BINARY:
            visit_expression(ctx, expr->binary.left);
            visit_expression(ctx, expr->binary.right);
            vm_emit_binary_arithmetic_op(ctx->fd, expr->binary.op);
            break;
    }
}

// Forward declaration for visit_statement
static const ast_stmt* visit_statements(vm_code_generation_traversal_context* ctx, const ast_stmt* stmts);

static void visit_stmt(vm_code_generation_traversal_context* ctx, const ast_stmt* stmt) {
    switch (stmt->kind) {
        case STMT_LET:
            sym_table_token token;
            if (!sym_table_find(ctx->symtab, &stmt->let_stmt.var_name, &token)) {
                ctx->had_error = true;
                return;
            }
            visit_expression(ctx, stmt->let_stmt.value);

            if (stmt->let_stmt.index) {
                vm_emit_push(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);
                visit_expression(ctx, stmt->let_stmt.index);
                vm_emit_binary_arithmetic_op(ctx->fd, BINARY_OP_ADD);

                // Calculate offset = array_base + index_offset
                vm_emit_pop(ctx->fd, SEGMENT_POINTER, POINTER_THAT);
                vm_emit_pop(ctx->fd, SEGMENT_THAT, 0);
            } else {
                vm_emit_pop(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);
            }
            break;
        case STMT_IF: {
            // Advance the counter immediately as the branches may have nested if statements
            uint32_t label_idx = ctx->if_label_index++;
            visit_expression(ctx, stmt->if_stmt.condition);

            vm_emit_if_goto(ctx->fd, IF_TRUE_LABEL, label_idx);
            // False branch
            visit_statements(ctx, stmt->if_stmt.false_branch);
            vm_emit_goto(ctx->fd, IF_END_LABEL, label_idx);

            // True branch
            vm_emit_label(ctx->fd, IF_TRUE_LABEL, label_idx);
            visit_statements(ctx, stmt->if_stmt.true_branch);

            vm_emit_label(ctx->fd, IF_END_LABEL, label_idx);
            break;
        }
        case STMT_WHILE: {
            // Advance the counter immediately as the branches may have nested while statements
            uint32_t label_idx = ctx->while_label_index++;
            vm_emit_label(ctx->fd, WHILE_CONDITION_LABEL, label_idx);

            visit_expression(ctx, stmt->while_stmt.cond);
            vm_emit_unary_arithmetic(ctx->fd, UNARY_OP_NOT);
            vm_emit_if_goto(ctx->fd, "WHILE_END", label_idx);

            visit_statements(ctx, stmt->while_stmt.body);
            vm_emit_goto(ctx->fd, WHILE_CONDITION_LABEL, label_idx);

            vm_emit_label(ctx->fd, WHILE_END_LABEL, label_idx);
            break;
        }
        case STMT_DO:
            visit_subroutine_call(ctx, stmt->do_stmt);
            break;
        case STMT_RETURN:
            if (stmt->return_stmt) {
                visit_expression(ctx, stmt->return_stmt);
            } else {
                vm_emit_signed_const(ctx->fd, 0);
            }
            vm_emit_return(ctx->fd);
            break;
    }
}

static const ast_stmt* visit_statements(vm_code_generation_traversal_context* ctx, const ast_stmt* stmts) {
    const ast_stmt* prev = nullptr;
    for (const ast_stmt* stmt = stmts; stmt; stmt = stmt->next) {
        visit_stmt(ctx, stmt);
        prev = stmt;
    }
    return prev;
}

static void visit_subroutine(vm_code_generation_traversal_context* ctx, const ast_subroutine* sub) {
    if (sub->is_native) {
        // Implemented elsewhere for example assembly.
        // Intended for std subroutines which cannot be implemented in Jack
        // Or are considered hot path and will benefit from hand optimized assembly
        // as jackc is not an optimizing compiler
        return;
    }
    ctx->symtab = sym_table_push(ctx->symtab, ctx->allocator);
    if (ctx->subroutine_signature.kind == SUB_METHOD) {
        // Not used directly, but is needed to keep the correct indexes
        register_var(ctx->symtab, ctx->this);
    }

    for (const ast_var_dec* argument = sub->params; argument; argument = argument->next) {
        register_var(ctx->symtab, argument);
    }
    for (const ast_var_dec* local = sub->locals; local; local = local->next) {
        register_var(ctx->symtab, local);
    }

    vm_emit_function(ctx->fd, &ctx->class_name, &ctx->subroutine_signature);
    switch (ctx->subroutine_signature.kind) {
        case SUB_CONSTRUCTOR:
            emit_std_call(ctx->fd, (std_subroutine_call){
                .kind = STD_MEMORY_ALLOC,
                .memory_alloc = {
                    .words_to_allocate = ctx->n_fields
                }
            });
            vm_emit_pop(ctx->fd, SEGMENT_POINTER, 0);
            break;
        case SUB_METHOD:
            vm_emit_push(ctx->fd, SEGMENT_ARG, 0);
            vm_emit_pop(ctx->fd, SEGMENT_POINTER, POINTER_THIS);
            break;
        case SUB_FUNCTION:
            break;
    }

    const ast_stmt* last_stmt = visit_statements(ctx, sub->body);
    // Add `return 0` to avoid fallthrough
    if (!last_stmt || last_stmt->kind != STMT_RETURN) {
        vm_emit_signed_const(ctx->fd, 0);
        vm_emit_return(ctx->fd);
    }

    ctx->symtab = sym_table_pop(ctx->symtab);
}

void vm_code_genetation_traversal(const ast_class* class, vm_code_generation_traversal_context* ctx) {
    for (const ast_var_dec* class_var = class->class_vars; class_var; class_var = class_var->next) {
        register_var(ctx->symtab, class_var);
    }
    ctx->class_name = class->name;

    uint16_t n_fields = 0;
    for (ast_var_dec* var = class->class_vars; var; var = var->next) {
        if (var->kind == VAR_FIELD)
            ++n_fields;
    }
    ctx->n_fields = n_fields;

    for (ast_subroutine* sub = class->subroutines; sub; sub = sub->next) {
        function_signature signature;
        if (!function_registry_find(ctx->registry, &ctx->class_name, &sub->name, &signature)) {
            // Should not happen as program is semantically correct
            ctx->had_error = true;
            continue;
        }
        ctx->subroutine_signature = signature;
        visit_subroutine(ctx, sub);
    }

    sym_table_reset_local_state(ctx->symtab);
}
