#ifndef JACKC_COMPILER_AST_TRAVERSALS_H
#define JACKC_COMPILER_AST_TRAVERSALS_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/symtable/compiler_symtable.h"

typedef struct {
    sym_table* symtab;
    function_registry* registry;
    jackc_diagnostic_engine engine;
    bool had_redeclaration;
} symtab_traversal_context;

void ast_symtab_build_traversal(ast_class* class, symtab_traversal_context* ctx);

#endif
