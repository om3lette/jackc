#include "compiler/ast/ast.h"
#include "compiler/ast/traversals.h"
#include "compiler/diagnostics-engine/diagnostic.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/symtable/compiler_symtable.h"
#include "compiler/symtable/symtable_token.h"
#include "core/asserts/jackc_assert.h"
#include "std/jackc_string.h"

#define INVALID_STATE(_ctx) _ctx->is_invalid = true

/**
 * Converts ast_type to jack_type
 *
 * @param type Type to convert
 * @param out  Pointer to write the resolved type to
 *
 * @return true if an error occured, false otherwise
 */
static bool ast_type_to_jack_type(const ast_type* type, jack_type* out) {
    switch (type->kind) {
        case TYPE_INT:
            *out = JACK_INT;
            break;
        case TYPE_CHAR:
            *out = JACK_CHAR;
            break;
        case TYPE_BOOLEAN:
            *out = JACK_BOOLEAN;
            break;
        case TYPE_CLASS:
            *out = JACK_CLASS;
            break;
        case TYPE_VOID:
            return true;
    }
    return false;
}

static bool convert_type_with_diagnostic(const ast_var_dec* var_dec, semantic_validity_traversal_context* ctx, jack_type* type) {
    if (ast_type_to_jack_type(&var_dec->type, type)) {
        jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_INCOMPLETE_TYPE, var_dec->name);
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
        return true;
    }

    return false;
}

static bool sym_table_insert_with_diagnostic(semantic_validity_traversal_context* ctx, sym_table_token* token) {
    switch (sym_table_insert(ctx->symtab, token)) {
        case SYMTAB_ALREADY_EXISTS:
            jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_REDEFINITION, token->name);

            sym_table_token previous_definition;
            jackc_assert(sym_table_find(ctx->symtab, &token->name, &previous_definition));

            jackc_diag_add_note(&d, DIAG_NOTE_PREVIOUS_DEFINITION_IS_HERE, previous_definition.name, ctx->allocator);
            jackc_diag_emit(&d);
            INVALID_STATE(ctx);
            return true;
        case SYMTAB_OK:
            return false;
    }
    return false;
}

static bool register_var(semantic_validity_traversal_context* ctx, const ast_var_dec* var_dec) {
    // Void is not a valid variable type
    jack_type type = JACK_CLASS; // JACK_CLASS is assigned to make the compiler happy
    if (convert_type_with_diagnostic(var_dec, ctx, &type))
        return true;

    // Class used as a variable type is not defined
    if (
        var_dec->type.kind == TYPE_CLASS
        && !function_registry_contains_class(ctx->registry, &var_dec->type.class_name, nullptr)
    ) {
        jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_USE_OF_UNDECLARED_IDENTIFIER, var_dec->type.class_name);
        jackc_diag_emit(&d);
        return true;
    }

    sym_table_token token = sym_table_token_init(
        type,
        var_dec->kind,
        &var_dec->name,
        var_dec->type.kind == TYPE_CLASS ? &var_dec->type.class_name : nullptr
    );
    if (sym_table_insert_with_diagnostic(ctx, &token))
        return true;

    return false;
}


static bool symtab_find_or_diagnostic(semantic_validity_traversal_context* ctx, jackc_string var_name, sym_table_token* out) {
    if (!sym_table_find(ctx->symtab, &var_name, out)) {
        jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_USE_OF_UNDECLARED_IDENTIFIER, var_name);
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
        return false;
    }

    return true;
}

static bool is_valid_var_or_diagnostic(semantic_validity_traversal_context* ctx, const jackc_string* var_name) {
    // Variable is not defined
    sym_table_token token;
    if (!symtab_find_or_diagnostic(ctx, *var_name, &token))
        return false;
    // Shadows a subroutine name in the same class
    // static int m;
    // function void m() {
    //     let m = 2; < ?
    //     do m();    < ?
    // }
    function_signature signature;
    if (function_registry_contains(ctx->registry, &ctx->class_name, var_name, &signature)) {
        jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_REDEFINITION, *var_name);
        jackc_diag_add_note(&d, DIAG_NOTE_PREVIOUS_DEFINITION_IS_HERE, signature.name, ctx->allocator);
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
        return false;
    }
    return true;
}

static bool function_registry_find_or_diagnostic(
    semantic_validity_traversal_context* ctx,
    const jackc_string* class_name,
    const jackc_string* sub_name,
    function_signature* out
) {
    if (!function_registry_contains(ctx->registry, class_name, sub_name, out)) {
        jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_CALL_TO_UNDECLARED_SUBROUTINE, *sub_name);
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
        return false;
    }

    return true;
}

static void matching_return_type_or_diagnostic(const ast_type* return_type, const jackc_string* span, semantic_validity_traversal_context* ctx) {
    if (ctx->sub_signature.return_type->kind != return_type->kind) {
        jackc_diag_builder d = jackc_diag_begin(
            &ctx->engine, DIAG_ERROR, DIAG_NON_VOID_SUBROUTINE_SHOULD_RETURN_A_VALUE, *span
        );
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
        return;
    }
}

static void visit_expression(const ast_expr* expr, semantic_validity_traversal_context* ctx);

static void visit_subroutine_call(const ast_call* call, semantic_validity_traversal_context* ctx) {
    jackc_string receiver_class = ctx->class_name;
    bool is_receiver_an_instance = (
        ctx->sub_signature.kind == SUB_METHOD
        || ctx->sub_signature.kind == SUB_CONSTRUCTOR
    ) && call->implicit_this_receiver;

    if (!call->implicit_this_receiver) {
        // var Solver x; x.solve()
        // First need to resolve x as an instance of Solver
        receiver_class = call->receiver;
        sym_table_token token;
        if (sym_table_find(ctx->symtab, &call->receiver, &token)) {
            if (token.type != JACK_CLASS) {
                jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_CALLED_OBJECT_TYPE_IS_NOT_A_CLASS, call->receiver);
                jackc_diag_emit(&d);
                INVALID_STATE(ctx);
                return;
            }
            // Class instance
            is_receiver_an_instance = true;
            receiver_class = token.class_name;
        } else {
            if (!function_registry_contains_class(ctx->registry, &receiver_class, nullptr)) {
                jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_USE_OF_UNDECLARED_IDENTIFIER, receiver_class);
                jackc_diag_emit(&d);
                INVALID_STATE(ctx);
                return;
            }
            // Class type
            is_receiver_an_instance = false;
        }
    }
    function_signature signature;
    if (
        !function_registry_find_or_diagnostic(
            ctx,
            &receiver_class,
            &call->subroutine_name,
            &signature
        )
    ) {
        return;
    }
    if (!is_receiver_an_instance && signature.kind == SUB_METHOD) {
        jackc_diag_builder d = jackc_diag_begin(
            &ctx->engine,
            DIAG_ERROR,
            DIAG_CANNOT_CALL_METHOD_WITHOUT_AN_OBJECT,
            call->subroutine_name
        );
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
    }

    // TODO: Precalc for ast_call?
    uint16_t n_args = 0;
    for (ast_expr_list* arg = call->args; arg; arg = arg->next) {
        visit_expression(arg->expr, ctx);
        ++n_args;
    }
    if (n_args != signature.n_args) {
        jackc_diag_builder d = jackc_diag_begin(
            &ctx->engine,
            DIAG_ERROR,
            n_args < signature.n_args
                ? DIAG_TOO_FEW_ARGUMENTS_TO_FUNCTION_CALL
                : DIAG_TOO_MANY_ARGUMENTS_TO_FUNCTION_CALL,
            call->subroutine_name
        );
        d.diag.data.subroutine_n_args_mismatch = (typeof(d.diag.data.subroutine_n_args_mismatch)) {
            .expected = signature.n_args,
            .got = n_args
        };
        // Diagnostics engine is only aware of the current source file
        if (jackc_string_cmp(&receiver_class, &ctx->class_name) == 0)
            jackc_diag_add_note(&d, DIAG_NOTE_DECLATED_HERE, signature.name, ctx->allocator);
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
    }
}

static void visit_expression(const ast_expr* expr, semantic_validity_traversal_context* ctx) {
    switch (expr->kind) {
        case EXPR_INT:
        case EXPR_STRING:
        case EXPR_KEYWORD:
            break;
        case EXPR_VAR:
            is_valid_var_or_diagnostic(ctx, &expr->var_name);
            break;
        case EXPR_ARRAY_ACCESS: // varName[expr]
            is_valid_var_or_diagnostic(ctx, &expr->array_access.var_name);
            visit_expression(expr->array_access.index, ctx);
            break;
        case EXPR_CALL:         // subroutineCall
            visit_subroutine_call(&expr->call, ctx);
            break;
        case EXPR_UNARY:
            visit_expression(expr->unary.operand, ctx);
            break;
        case EXPR_BINARY:
            visit_expression(expr->binary.left, ctx);
            visit_expression(expr->binary.right, ctx);
            break;
    }
}

// Forward declaration for visit_statement
static void visit_statements(const ast_stmt* stmts, semantic_validity_traversal_context* ctx);

static void visit_statement(const ast_stmt* stmt, semantic_validity_traversal_context* ctx) {
    switch (stmt->kind) {
        case STMT_LET:
            is_valid_var_or_diagnostic(ctx, &stmt->let_stmt.var_name);

            if (stmt->let_stmt.value->kind == EXPR_STRING) {
                sym_table_token token;
                // Already validated
                symtab_find_or_diagnostic(ctx, stmt->let_stmt.var_name, &token);
                if (
                    token.type != JACK_CLASS
                    ||(
                        !jackc_streq(&token.class_name, "String")
                        && !(jackc_streq(&token.class_name, "Array") && stmt->let_stmt.index)
                    )
                ) {
                    jackc_diag_builder d = jackc_diag_begin(
                        &ctx->engine,
                        DIAG_ERROR,
                        DIAG_INVALID_OPERATION,
                        jackc_string_create(
                            stmt->node.span.data,
                            (size_t)(stmt->let_stmt.value->node.span.data - stmt->node.span.data) + stmt->let_stmt.value->node.span.length + 1
                        )
                    );
                    jackc_diag_add_note(&d, DIAG_NOTE_DECLATED_HERE, token.name, ctx->allocator);
                    jackc_diag_emit(&d);
                    INVALID_STATE(ctx);
                }
            }
            if (stmt->let_stmt.index) visit_expression(stmt->let_stmt.index, ctx);
            visit_expression(stmt->let_stmt.value, ctx);
            break;
        case STMT_IF:
            visit_expression(stmt->if_stmt.condition, ctx);
            if (!stmt->if_stmt.true_branch && !stmt->if_stmt.false_branch) {
                jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_WARNING, DIAG_EMPTY_IF_STATEMENT, stmt->node.span);
                jackc_diag_emit(&d);
                INVALID_STATE(ctx);
            } else {
                visit_statements(stmt->if_stmt.true_branch, ctx);
                visit_statements(stmt->if_stmt.false_branch, ctx);
            }
            break;
        case STMT_WHILE:
            visit_expression(stmt->while_stmt.cond, ctx);
            visit_statements(stmt->while_stmt.body, ctx);
            break;
        case STMT_DO:
            visit_subroutine_call(stmt->do_stmt, ctx);
            break;
        case STMT_RETURN: {
            ctx->has_return_stmt = true;
            if (stmt->return_stmt) {
                // TODO: Check for matching types?
                // TODO: Check for constructor returning the expected class
                visit_expression(stmt->return_stmt, ctx);
            } else {
                // No return expression -> void returning void
                matching_return_type_or_diagnostic(&(ast_type) {.kind = TYPE_VOID}, &stmt->node.span, ctx);
            }
            break;
        }
    }
}

static void visit_statements(const ast_stmt* stmts, semantic_validity_traversal_context* ctx) {
    for (const ast_stmt* stmt = stmts; stmt; stmt = stmt->next) {
        visit_statement(stmt, ctx);
    }
}

static void visit_subroutine(const ast_subroutine* sub, semantic_validity_traversal_context* ctx) {
    ctx->symtab = sym_table_push(ctx->symtab, ctx->allocator);

    // TODO: Make a separate function
    ctx->subroutine_name = sub->name;
    ctx->has_constructor |= sub->kind == SUB_CONSTRUCTOR;
    ctx->has_dispose_method |= jackc_streq(&sub->name, "dispose");
    ctx->has_return_stmt = false;
    if (!function_registry_find_or_diagnostic(ctx, &ctx->class_name, &ctx->subroutine_name, &ctx->sub_signature))
        return;

    // Register arguments
    for (const ast_var_dec* argument = sub->params; argument; argument = argument->next) {
        if (register_var(ctx, argument))
            INVALID_STATE(ctx);
    }
    // Register local vars
    for (const ast_var_dec* local_var_dec = sub->locals; local_var_dec; local_var_dec = local_var_dec->next) {
        if (register_var(ctx, local_var_dec))
            INVALID_STATE(ctx);
    }
    visit_statements(sub->body, ctx);

    // Native subroutines are declared somewhere else
    // Therefore have no return statement and will always trigger a false error
    if (!ctx->has_return_stmt && !sub->is_native) {
        matching_return_type_or_diagnostic(&(ast_type){.kind = TYPE_VOID}, &ctx->subroutine_name, ctx);
    }

    ctx->symtab = sym_table_pop(ctx->symtab);
}

bool ast_semantic_validity_traversal(const ast_class* class, semantic_validity_traversal_context* ctx) {
    if (!function_registry_contains_class(ctx->registry, &class->name, nullptr)) {
        jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_USE_OF_UNDECLARED_IDENTIFIER, class->name);
        jackc_diag_emit(&d);
        INVALID_STATE(ctx);
        return true;
    }

    ctx->symtab = sym_table_push(ctx->symtab, ctx->allocator);
    for (const ast_var_dec* class_var = class->class_vars; class_var; class_var = class_var->next) {
        jackc_assert(class_var->kind == VAR_STATIC || class_var->kind == VAR_FIELD);
        if (register_var(ctx, class_var))
            INVALID_STATE(ctx);
    }

    for (ast_subroutine* sub = class->subroutines; sub; sub = sub->next) {
        visit_subroutine(sub, ctx);
    }
    ctx->symtab = sym_table_pop(ctx->symtab);

    if (ctx->has_constructor && !ctx->has_dispose_method) {
        jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_WARNING, DIAG_WARNING_CONSTRUCTOR_WITH_NO_DISPOSE, class->name);
        jackc_diag_emit(&d);
    }

    return ctx->is_invalid;
}
