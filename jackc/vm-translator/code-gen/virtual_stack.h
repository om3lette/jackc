#ifndef JACKC_VIRTUAL_STACK_H
#define JACKC_VIRTUAL_STACK_H

#include "vm-translator/code-gen/asm_emit.h"
#include "vm-translator/code-gen/stack_frame.h"

typedef struct {
    const emitter* emit;
    const frame_config* cfg;
    const char* sp_reg;
} vstack;

[[ nodiscard ]] vstack vstack_init(emitter* e, const frame_config* cfg, const char* sp_reg);

/**
 * Stack alloc and sw
 */
void vstack_push_reg(const vstack* s, const char* reg);

/**
 * sw and stack dealloc
 */
void vstack_pop_reg(const vstack* s, const char* reg);

/**
 * Raw lw with no sp adjustment
 */
void vstack_peek_reg(const vstack* s, const char* reg, int from_top);

/**
 * Raw sw with no sp adjustment
 */
void vstack_poke_reg(const vstack* s, const char* reg, int word_offset_from_top);

/**
 * Raw sp movement
 */
void vstack_adjust(const vstack* s, int delta_words);


void vstack_push_imm(const vstack* s, int32_t value);
void vstack_push_mem(const vstack* s, const char* base, int word_offset);
void vstack_pop_mem(const vstack* s, const char* base, int word_offset);
void vstack_alloc(const vstack* s, uint32_t n_words);
void vstack_dealloc(const vstack* s, uint32_t n_words);

#endif
