#include "ast.h"
#include "compiler/lexer/compiler_lexer.h"
#include "jackc_string.h"

static void ast_base_init(ast_base* base, const jack_location* loc) {
    base->loc = *loc;
}

static ast_expr* ast_expr_common_init(
    Allocator* allocator,
    jack_location* loc,
    ast_expr_kind kind
) {
    ast_expr* expr = allocator->alloc(sizeof(ast_expr), allocator->context);
    ast_base_init(&expr->base, loc);

    expr->kind = kind;
    return expr;
}

ast_expr* ast_expr_int(
    Allocator* allocator,
    jack_location* loc,
    int32_t value
) {
    ast_expr* expr = ast_expr_common_init(allocator, loc, EXPR_INT);
    expr->int_val = value;

    return expr;
}

ast_expr* ast_expr_string(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* value
) {
    ast_expr* expr = ast_expr_common_init(allocator, loc, EXPR_STRING);
    expr->string_val = *value;

    return expr;
}

ast_expr* ast_expr_var(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* name
) {
    ast_expr* expr = ast_expr_common_init(allocator, loc, EXPR_VAR);
    expr->var_name = *name;

    return expr;
}

ast_expr* ast_expr_binary(
    Allocator* allocator,
    jack_location* loc,
    ast_expr* left,
    ast_binary_op op,
    ast_expr* right
) {
    ast_expr* expr = ast_expr_common_init(allocator, loc, EXPR_BINARY);

    expr->binary.left = left;
    expr->binary.op = op;
    expr->binary.right = right;

    return expr;
}

ast_expr* ast_expr_unary(
    Allocator* allocator,
    jack_location* loc,
    ast_unary_op op,
    ast_expr* operand
) {
    ast_expr* expr = ast_expr_common_init(allocator, loc, EXPR_UNARY);

    expr->unary.operand = operand;
    expr->unary.op = op;

    return expr;
}

ast_expr* ast_expr_call(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* receiver,
    const jackc_string* subroutine_name,
    ast_expr_list* args
) {
    ast_expr* expr = ast_expr_common_init(allocator, loc, EXPR_CALL);

    expr->call.args = args;
    expr->call.subroutine_name = *subroutine_name;

    expr->call.implicit_this_receiver = !receiver;
    expr->call.receiver = *receiver;

    return expr;
}

ast_expr* ast_expr_array_access(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* var_name,
    ast_expr* index
) {
    ast_expr* expr = ast_expr_common_init(allocator, loc, EXPR_ARRAY_ACCESS);

    expr->array_access.var_name = *var_name;
    expr->array_access.index = index;

    return expr;
}


static ast_stmt* ast_stmt_common_init(
    Allocator* allocator,
    jack_location* loc,
    ast_stmt_kind kind
) {
    ast_stmt* stmt = allocator->alloc(sizeof(ast_stmt), allocator->context);
    ast_base_init(&stmt->base, loc);

    stmt->kind = kind;
    stmt->next = nullptr;
    return stmt;
}

ast_stmt* ast_stmt_let(
    Allocator* a,
    jack_location* loc,
    const jackc_string* var_name,
    ast_expr* index,
    ast_expr* value
) {
    ast_stmt* stmt = ast_stmt_common_init(a, loc, STMT_LET);

    stmt->let_stmt.index = index;
    stmt->let_stmt.value = value;
    stmt->let_stmt.var_name = *var_name;

    return stmt;
}

ast_stmt* ast_stmt_if(
    Allocator* a,
    jack_location* loc,
    ast_expr* cond,
    ast_stmt* true_branch,
    ast_stmt* false_branch
) {
    ast_stmt* stmt = ast_stmt_common_init(a, loc, STMT_IF);

    stmt->if_stmt.cond = cond;
    stmt->if_stmt.true_branch = true_branch;
    stmt->if_stmt.false_branch = false_branch;

    return stmt;
}

ast_stmt* ast_stmt_while(
    Allocator* a,
    jack_location* loc,
    ast_expr* cond,
    ast_stmt* body
) {
    ast_stmt* stmt = ast_stmt_common_init(a, loc, STMT_WHILE);

    stmt->while_stmt.cond = cond;
    stmt->while_stmt.body = body;

    return stmt;
}


ast_stmt* ast_stmt_do(
    Allocator* a,
    jack_location* loc,
    const jackc_string* receiver,
    const jackc_string* subroutine_name,
    ast_expr_list* args
) {
    ast_stmt* stmt = ast_stmt_common_init(a, loc, STMT_DO);

    stmt->do_stmt.args = args;
    stmt->do_stmt.subroutine_name = *subroutine_name;

    stmt->do_stmt.implicit_this_receiver = !receiver;
    stmt->do_stmt.receiver = *receiver;

    return stmt;
}


ast_stmt* ast_stmt_return(
    Allocator* a,
    jack_location* loc,
    ast_expr* value
) {
    ast_stmt* stmt = ast_stmt_common_init(a, loc, STMT_DO);

    stmt->return_stmt = value;

   return stmt;
}

ast_var_dec* ast_variable_declaration(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* name,
    jack_variable_kind kind,
    ast_type type,
    ast_var_dec* next
) {
    ast_var_dec* var = allocator->alloc(sizeof(ast_var_dec), allocator->context);
    ast_base_init(&var->base, loc);

    var->kind = kind;
    var->type = type;
    var->next = next;
    var->name = *name;

    return var;
}

ast_subroutine* ast_subroutine_create(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* name,
    ast_var_dec* params,
    ast_var_dec* locals,
    ast_stmt* body,
    ast_subroutine* next
) {
    ast_subroutine* subroutine = allocator->alloc(sizeof(ast_subroutine), allocator->context);
    ast_base_init(&subroutine->base, loc);

    subroutine->params = params;
    subroutine->locals = locals;
    subroutine->body = body;
    subroutine->next = next;
    subroutine->name = *name;

    return subroutine;
}

ast_class* ast_class_create(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* name,
    ast_var_dec* class_vars,
    ast_subroutine* subroutines
) {
    ast_class* class = allocator->alloc(sizeof(ast_class), allocator->context);
    ast_base_init(&class->base, loc);

    class->class_vars = class_vars;
    class->subroutines = subroutines;
    class->name = *name;

    return class;
}
