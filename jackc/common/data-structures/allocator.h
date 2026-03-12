#ifndef DATA_STRUCTURES_ALLOCATOR_H
#define DATA_STRUCTURES_ALLOCATOR_H

#include <stddef.h>

#define ALIGN_UP(round, to_multiple_of) (((round) + ((to_multiple_of) - 1)) & ~((to_multiple_of) - 1))

#ifdef __rars__
#   define ALIGNMENT 4
#else
#   define ALIGNMENT alignof(max_align_t)
#endif

typedef struct {
    void* (*alloc)(size_t bytes, void* context);
    void (*free)(void* ptr, size_t bytes, void* context);
    void* context;
} Allocator;

#endif
