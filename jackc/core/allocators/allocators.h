#ifndef JACKC_ALLOCATORS_H
#define JACKC_ALLOCATORS_H

#include <stddef.h>
#include <stdalign.h>

#define ALIGN_UP(round, to_multiple_of) (((round) + ((to_multiple_of) - 1)) & ~((to_multiple_of) - 1))

#ifdef __rars__
#   define ALIGNMENT (unsigned int)4
#else
#   define ALIGNMENT alignof(max_align_t)
#endif

#ifndef ARENA_START_BLOCK_SIZE
#   define ARENA_START_BLOCK_SIZE 65536
#endif
static_assert((ARENA_START_BLOCK_SIZE & (ARENA_START_BLOCK_SIZE - 1)) == 0, "ARENA_START_BLOCK_SIZE must be a power of 2");

typedef struct {
    void* (*alloc)(size_t bytes, void* context);
    void (*free)(void* ptr, size_t bytes, void* context);
    void* context;
} Allocator;

typedef struct {
    struct arena_block* next;
    size_t size, capacity;
    alignas(ALIGNMENT) unsigned char data[];
} arena_block;

typedef struct {
    arena_block* current_block;
} arena_allocator;

[[ nodiscard ]] arena_allocator* arena_allocator_init();

[[ nodiscard ]] void* arena_allocator_alloc(arena_allocator* allocator, size_t bytes);

void arena_allocator_destroy(arena_allocator* allocator);

#endif
