#include "function_registry.h"
#include "core/data-structures/hashmap.h"
#include "core/hash.h"

static hash_t function_registry_key_hasher(const void* key) {
    const jackc_string* kkey = key;
    return oat_hash(kkey->data, kkey->length);
}

static int function_registry_key_comparator(const void* a, const void* b) {
    return jackc_string_cmp(a, b);
}

function_registry* function_registry_init(Allocator* allocator) {
    function_registry* registry = allocator->alloc(sizeof(function_registry), allocator->context);
    registry->classes = fixed_hashmap_init(
        jackc_string, class_symbol,
        function_registry_key_hasher,
        function_registry_key_comparator,
        allocator
    );
    registry->allocator = allocator;
    return registry;
}

sym_table_return_code function_registry_insert(function_registry* registry, const ast_class* class) {
    if (function_registry_find_class(registry, &class->name, nullptr))
        return SYMTAB_ALREADY_EXISTS;

    class_symbol symbol;
    symbol.methods = fixed_hashmap_init(
        jackc_string, function_signature,
        function_registry_key_hasher,
        function_registry_key_comparator,
        registry->allocator
    )
    fixed_hashmap_insert(registry->classes, &class->name, &symbol);

    for (ast_subroutine* sub = class->subroutines; sub; sub = sub->next) {
        uint16_t n_args = 0;
        for (const ast_var_dec* arg = sub->params; arg; arg = arg->next) {
            ++n_args;
        }
        uint16_t n_locals = 0;
        for (const ast_var_dec* local = sub->locals; local; local = local->next) {
            ++n_locals;
        }
        function_signature signature = {
            .name = sub->name,
            .kind = sub->kind,
            .n_args = n_args,
            .n_locals = n_locals,
            .arguments = sub->params,
            .return_type = &sub->return_type,
        };
        fixed_hashmap_insert(symbol.methods, &sub->name, &signature);
    }
    return SYMTAB_OK;
}

bool function_registry_find_class(
    const function_registry* registry,
    const jackc_string* class_name,
    class_symbol* out
) {
    return fixed_hashmap_find(registry->classes, class_name, out);
}

bool function_registry_find(
    const function_registry* registry,
    const jackc_string* class_name,
    const jackc_string* method_name,
    function_signature* found
) {
    class_symbol cls_symbol;
    if (!function_registry_find_class(registry, class_name, &cls_symbol))
        return false;

    return fixed_hashmap_find(cls_symbol.methods, method_name, found);
}
