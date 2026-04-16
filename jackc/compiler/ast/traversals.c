#include "traversals.h"
#include "compiler/function-registry/function_registry.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"

semantic_validity_traversal_context semantic_validity_traversal_context_init(
    const function_registry* registry,
    jackc_diagnostic_engine* engine,
    Allocator* allocator
) {
    semantic_validity_traversal_context ctx = (semantic_validity_traversal_context) {
        .class_name = {0},
        .subroutine_name = {0},

        .symtab = nullptr,
        .registry = registry,
        .engine = *engine,

        .allocator = allocator,

        .is_invalid = false,
        .has_constructor = false,
        .has_dispose_method = false,
        .has_return_stmt = false
    };
    jackc_memset(&ctx.sub_signature, sizeof(function_signature), 0);
    return ctx;
}


void semantic_validity_enter_class(semantic_validity_traversal_context* ctx, const ast_class* class) {
    ctx->symtab = sym_table_push(ctx->symtab, ctx->allocator);
    ctx->class_name = class->name;
    ctx->class_span = class->node.span;
}

void semantic_validity_enter_subroutine(semantic_validity_traversal_context* ctx, const ast_subroutine* sub) {
    ctx->subroutine_name = sub->name;
    ctx->subroutine_span = sub->node.span;
    ctx->has_constructor |= sub->kind == SUB_CONSTRUCTOR;
    ctx->has_dispose_method |= jackc_streq(&sub->name, "dispose");
    ctx->has_return_stmt = false;
}
