#ifndef JACKC_COMPILER_FUNCTION_REGISTRY_H
#define JACKC_COMPILER_FUNCTION_REGISTRY_H

#include "compiler/ast/ast.h"
#include "compiler/symtable/compiler_symtable.h"
#include "core/data-structures/hashmap.h"

typedef struct {
    fixed_hash_map* classes;
    Allocator* allocator;
} function_registry;

typedef struct {
    fixed_hash_map* methods;
} class_symbol;

typedef struct {
    ast_sub_kind kind;
    ast_var_dec* arguments;
    ast_type* return_type;
} function_signature;

function_registry* function_registry_init(Allocator* allocator);

sym_table_return_code function_registry_insert(function_registry* registry, const ast_class* class);

bool function_registry_contains_class(
    function_registry* registry,
    const jackc_string* class_name,
    class_symbol* out
);

bool function_registry_contains(
    function_registry* registry,
    const jackc_string* class_name,
    const jackc_string* method_name,
    function_signature* found
);

// TODO: Normalize api of symtab and function registry (contains/find etc)
#endif
