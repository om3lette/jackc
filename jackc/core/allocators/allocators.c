#include "allocators.h"
#include "core/asserts/jackc_assert.h"
#include "std/jackc_stdlib.h"

arena_allocator* arena_allocator_init() {
    arena_allocator* allocator = jackc_alloc(sizeof(arena_allocator));
    allocator->current_block = jackc_alloc(sizeof(arena_block) + ARENA_START_BLOCK_SIZE);
    allocator->current_block->next = nullptr;
    allocator->current_block->size = 0;
    allocator->current_block->capacity = ARENA_START_BLOCK_SIZE;

    return allocator;
}

void* arena_allocator_alloc(arena_allocator* allocator, size_t bytes) {
    jackc_assert(bytes > 0 && "Cannot allocate 0 bytes");

    if (allocator->current_block->capacity < allocator->current_block->size + bytes) {
        size_t nextBlockCapacity = allocator->current_block->capacity * 2;
        while (nextBlockCapacity < bytes) {
            nextBlockCapacity <<= 1;
        }

        arena_block* new_block = jackc_alloc(sizeof(arena_block) + nextBlockCapacity);
        new_block->next = (struct arena_block*)allocator->current_block;
        new_block->size = 0;
        new_block->capacity = nextBlockCapacity;
        allocator->current_block = new_block;
    }

    void* ptr = allocator->current_block->data + allocator->current_block->size;
    allocator->current_block->size = MIN(
        allocator->current_block->size + ALIGN_UP(bytes, ALIGNMENT),
        allocator->current_block->capacity
    );
    return ptr;
}

void arena_allocator_destroy(arena_allocator* allocator) {
    arena_block* block = allocator->current_block;
    while (block) {
        arena_block* next = (arena_block*)block->next;
        jackc_free(block);
        block = next;
    }
    jackc_free(allocator);
}
