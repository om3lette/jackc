#ifndef JACKC_TESTS_COMPILER_FUNCTION_REGISTRY_UTILS_H
#define JACKC_TESTS_COMPILER_FUNCTION_REGISTRY_UTILS_H

#include "compiler/function-registry/function_registry.h"
#include "core/allocators/allocators.h"
#include "core/allocators/adapters.h"

struct function_registry_fixture {
    Allocator allocator;
    function_registry* registry;
};

static inline void test_function_registry_common_setup(struct function_registry_fixture* tau) {
    tau->allocator = arena_allocator_adapter();
    tau->registry = function_registry_init(&tau->allocator);
}

static inline void test_function_registry_common_teardown(struct function_registry_fixture* tau) {
    arena_allocator_destroy(tau->allocator.context);
}

#endif
