#include "traversals.h"
#include "compiler/function-registry/function_registry.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"

semantic_validity_traversal_context semantic_validity_traversal_context_init(
    const jackc_string* class_name,
    const function_registry* registry,
    jackc_diagnostic_engine* engine,
    Allocator* allocator
) {
    semantic_validity_traversal_context ctx = (semantic_validity_traversal_context) {
        .class_name = *class_name,

        .subroutine_name = jackc_string_empty(),

        .symtab = nullptr,
        .registry = registry,
        .engine = *engine,

        .allocator = allocator,

        .is_invalid = false,
        .has_constructor = false,
        .has_dispose_method = false
    };
    jackc_memset(&ctx.sub_signature, sizeof(function_signature), 0);
    return ctx;
}
