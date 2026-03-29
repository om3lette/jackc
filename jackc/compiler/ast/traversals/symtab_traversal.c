#include "compiler/ast/traversals.h"
#include "compiler/diagnostics-engine/diagnostic.h"
#include "compiler/symtable/compiler_symtable.h"

void ast_function_registry_build_traversal(const ast_class* class, function_registry_traversal_context* ctx) {
    switch (function_registry_insert(ctx->registry, class)) {
        case SYMTAB_ALREADY_EXISTS:
            ctx->had_redeclaration = true;
            jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_REDEFINITION, class->name);
            jackc_diag_emit(&d);
            break;
        case SYMTAB_OK:
            function_registry_insert(ctx->registry, class);
            break;
    }
}
