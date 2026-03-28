#include "symtab_traversal.h"
#include "compiler/diagnostics-engine/diagnostic.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/symtable/compiler_symtable.h"
#include "compiler/symtable/symtable_token.h"

void ast_symtab_build_traversal(ast_class* class, symtab_traversal_context* ctx) {
    sym_table_token token = sym_table_token_init(
        JACK_CLASS,
        VAR_STATIC,
        &class->name,
        &class->name
    );

    switch (sym_table_insert(ctx->symtab, &token)) {
        case SYMTAB_ALREADY_EXISTS:
            ctx->had_redeclaration = true;
            jackc_diag_builder d = jackc_diag_begin(&ctx->engine, DIAG_ERROR, DIAG_REDEFINITION_OF_CLASS, class->name);
            jackc_diag_emit(&d);
            break;
        case SYMTAB_OK:
            function_registry_insert(ctx->registry, class);
            break;
    }
}
