#ifndef JACKC_COMPILER_FUNCTION_REGISTRY_H
#define JACKC_COMPILER_FUNCTION_REGISTRY_H

#include "compiler/ast/ast.h"
#include "compiler/symtable/compiler_symtable.h"
#include "core/data-structures/hashmap.h"
#include <stdint.h>

typedef struct {
    fixed_hash_map* classes;
    Allocator* allocator;
} function_registry;

typedef struct {
    fixed_hash_map* methods;
} class_symbol;

typedef struct {
    jackc_string name;
    ast_sub_kind kind;
    uint16_t n_args;
    uint16_t n_locals;
    ast_var_dec* arguments;
    ast_type* return_type;
} function_signature;

function_registry* function_registry_init(Allocator* allocator);

sym_table_return_code function_registry_insert(function_registry* registry, const ast_class* class);

bool function_registry_find_class(
    const function_registry* registry,
    const jackc_string* class_name,
    class_symbol* out
);

bool function_registry_find(
    const function_registry* registry,
    const jackc_string* class_name,
    const jackc_string* method_name,
    function_signature* found
);

#endif
