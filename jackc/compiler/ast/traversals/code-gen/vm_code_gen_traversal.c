#include "compiler/ast/ast.h"
#include "compiler/ast/traversals.h"
#include "compiler/ast/traversals/code-gen/std_utils.h"
#include "compiler/ast/traversals/code-gen/code_gen_utils.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/symtable/compiler_symtable.h"
#include "compiler/symtable/symtable_token.h"
#include "core/asserts/jackc_assert.h"
#include "vm-translator/constants.h"
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
    // Will not happen
    return JACK_CLASS;
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
    // TODO: Add support for stack rotation?
    uint16_t n_args = 0;
    for (const ast_expr_list* arg = call->args; arg; arg = arg->next) {
        visit_expression(ctx, arg->expr);
        ++n_args;
    }

    const jackc_string* receiver = &call->receiver;

    sym_table_token receiver_var;
    if (call->implicit_this_receiver) {
        receiver = &ctx->class_name;
    } else if (sym_table_find(ctx->symtab, receiver, &receiver_var)) {
        // receiver is a variable of class type -> resolve to the class name
        jackc_assert(receiver_var.type == JACK_CLASS);
        receiver = &receiver_var.class_name;
    }
    emit_call(ctx->fd, receiver, &call->subroutine_name, n_args);
}

static void visit_expression(vm_code_generation_traversal_context* ctx, const ast_expr* expr) {
    switch (expr->kind) {
        case EXPR_INT:
            emit_signed_const(ctx->fd, expr->int_val);
            break;
        case EXPR_STRING:
            emit_string_from_string_literal(ctx->fd, &expr->string_val);
            break;
        case EXPR_KEYWORD:
            switch (expr->keyword_val) {
                case KEYWORD_TRUE:
                    emit_signed_const(ctx->fd, 1);
                    break;
                case KEYWORD_FALSE:
                case KEYWORD_NULL:
                    emit_signed_const(ctx->fd, 0);
                    break;
                case KEYWORD_THIS:
                    emit_push(ctx->fd, SEGMENT_POINTER, 0);
                    break;
            }
            break;
        case EXPR_VAR: {
            sym_table_token token;
            if (!sym_table_find(ctx->symtab, &expr->var_name, &token)) {
                ctx->had_error = true;
                return;
            }
            emit_push(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);
            break;
        }
        case EXPR_ARRAY_ACCESS: {
            sym_table_token token;
            if (!sym_table_find(ctx->symtab, &expr->array_access.var_name, &token)) {
                ctx->had_error = true;
                return;
            }
            emit_push(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);

            visit_expression(ctx, expr->array_access.index);
            emit_push(ctx->fd, SEGMENT_CONSTANT, 4);
            emit_binary_arithmetic_op(ctx->fd, BINARY_OP_MUL);

            emit_binary_arithmetic_op(ctx->fd, BINARY_OP_ADD);
            emit_pop(ctx->fd, SEGMENT_POINTER, POINTER_THAT);
            emit_push(ctx->fd, SEGMENT_THAT, 0);
            break;
        }
        case EXPR_CALL: {
            visit_subroutine_call(ctx, &expr->call);
            break;
        }
        case EXPR_UNARY:
        visit_expression(ctx, expr->unary.operand);
            emit_unary_arithmetic(ctx->fd, expr->unary.op);
            break;
        case EXPR_BINARY:
            visit_expression(ctx, expr->binary.left);
            visit_expression(ctx, expr->binary.right);
            emit_binary_arithmetic_op(ctx->fd, expr->binary.op);
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
                emit_push(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);

                // Compute index offset from base ptr
                visit_expression(ctx, stmt->let_stmt.index);
                emit_push(ctx->fd, SEGMENT_CONSTANT, 4);
                emit_binary_arithmetic_op(ctx->fd, BINARY_OP_MUL);

                // Calculate offset = array_base + index_offset
                emit_binary_arithmetic_op(ctx->fd, BINARY_OP_ADD);
                emit_pop(ctx->fd, SEGMENT_POINTER, POINTER_THAT);
                emit_pop(ctx->fd, SEGMENT_THAT, 0);
            } else {
                emit_pop(ctx->fd, vm_segment_from_variable_kind(token.var.kind), token.var.idx);
            }
            break;
        case STMT_IF: {
            // Advance the counter immediately as the branches may have nested if statements
            uint32_t label_idx = ctx->if_label_index++;
            visit_expression(ctx, stmt->if_stmt.condition);

            emit_if_goto(ctx->fd, IF_TRUE_LABEL, label_idx);
            // False branch
            visit_statements(ctx, stmt->if_stmt.false_branch);
            emit_goto(ctx->fd, IF_END_LABEL, label_idx);

            // True branch
            emit_label(ctx->fd, IF_TRUE_LABEL, label_idx);
            visit_statements(ctx, stmt->if_stmt.true_branch);

            emit_label(ctx->fd, IF_END_LABEL, label_idx);
            break;
        }
        case STMT_WHILE: {
            // Advance the counter immediately as the branches may have nested while statements
            uint32_t label_idx = ctx->while_label_index++;
            emit_label(ctx->fd, WHILE_CONDITION_LABEL, label_idx);

            visit_expression(ctx, stmt->while_stmt.cond);
            emit_unary_arithmetic(ctx->fd, UNARY_OP_NOT);
            emit_if_goto(ctx->fd, "WHILE_END", label_idx);

            visit_statements(ctx, stmt->while_stmt.body);
            emit_goto(ctx->fd, WHILE_CONDITION_LABEL, label_idx);

            emit_label(ctx->fd, WHILE_END_LABEL, label_idx);
            break;
        }
        case STMT_DO:
            visit_subroutine_call(ctx, stmt->do_stmt);
            break;
        case STMT_RETURN:
            if (stmt->return_stmt) visit_expression(ctx, stmt->return_stmt);
            emit_return(ctx->fd);
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
    ctx->symtab = sym_table_push(ctx->symtab, ctx->allocator);
    for (const ast_var_dec* argument = sub->params; argument; argument = argument->next) {
        register_var(ctx->symtab, argument);
    }
    for (const ast_var_dec* local = sub->locals; local; local = local->next) {
        register_var(ctx->symtab, local);
    }

    emit_function(ctx->fd, &ctx->class_name, &ctx->subroutine_signature);
    if (ctx->subroutine_signature.kind == SUB_CONSTRUCTOR) {
        emit_std_call(ctx->fd, (std_subroutine_call){
            .kind = STD_MEMORY_ALLOC,
            .memory_alloc = {
                .words_to_allocate = ctx->n_fields
            }
        });
        emit_pop(ctx->fd, SEGMENT_POINTER, 0);
    }
    const ast_stmt* last_stmt = visit_statements(ctx, sub->body);
    // Add return to avoid fallthrough
    if (!last_stmt || last_stmt->kind != STMT_RETURN) {
        emit_return(ctx->fd);
    }

    ctx->symtab = sym_table_pop(ctx->symtab);
}

void vm_code_genetation_traversal(const ast_class* class, vm_code_generation_traversal_context* ctx) {
    ctx->symtab = sym_table_init(nullptr, ctx->allocator);
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
        if (!function_registry_contains(ctx->registry, &ctx->class_name, &sub->name, &signature)) {
            // Should not happen as program is semantically correct
            ctx->had_error = true;
            continue;
        }
        ctx->subroutine_signature = signature;
        visit_subroutine(ctx, sub);
    }

    sym_table_pop(ctx->symtab);
}
