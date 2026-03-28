#include "allocators.h"

static void* arena_alloc(size_t bytes, void* context) {
    return arena_allocator_alloc(context, bytes);
}

static void arena_free(void* ptr, size_t bytes, void* context) {
    (void)ptr, (void)bytes, (void)context;
}

Allocator arena_allocator_adapter() {
    return (Allocator){
        .alloc = arena_alloc,
        .free = arena_free,
        .context = arena_allocator_init(),
    };
}
