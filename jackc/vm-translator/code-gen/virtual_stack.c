#include "vm-translator/code-gen/virtual_stack.h"
#include "std/jackc_stdlib.h"
#include "vm-translator/code-gen/asm_emit.h"
#include "vm-translator/code-gen/regs.h"
#include "vm-translator/code-gen/stack_frame.h"

vstack vstack_init(emitter* e, const frame_config* cfg, const char* sp_reg) {
    return (vstack){
        .emit = e,
        .cfg = cfg,
        .sp_reg = sp_reg,
    };
}

void vstack_adjust(const vstack* s, int delta_words) {
    int32_t bytes_to_alloc = frame_offset_bytes(s->cfg, delta_words);
    asm_emit_addi(s->emit, REG_SP, REG_SP, -bytes_to_alloc);
}

void vstack_peek_reg(const vstack* s, const char* reg, int word_offset_from_top) {
    int byte_offset = frame_offset_bytes(s->cfg, word_offset_from_top);
    asm_emit_lw(s->emit, reg, byte_offset, REG_SP);
}

void vstack_poke_reg(const vstack* s, const char* reg, int word_offset_from_top) {
    int byte_offset = frame_offset_bytes(s->cfg, word_offset_from_top);
    asm_emit_sw(s->emit, reg, byte_offset, REG_SP);
}

void vstack_push_reg(const vstack* s, const char* reg) {
    vstack_adjust(s, 1);
    vstack_poke_reg(s, reg, 0);
}

void vstack_pop_reg(const vstack* s, const char* reg) {
    vstack_peek_reg(s, reg, 0);
    vstack_adjust(s, -1);
}


void vstack_push_imm(const vstack* s, int32_t value) {
    asm_emit_li(s->emit, REG_SCRATCH, value);
    vstack_push_reg(s, REG_SCRATCH);
}

/**
 * Checks if the section that the register belongs to is not stack-based.
 * If this is the case than the byte offset should always be positive as those sections
 * grow upwards, unlike the stack, which grows downwards by default.
 */
static bool is_section_not_stack_based(const char* reg) {
    return jackc_strcmp(reg, REG_STATIC) == 0
    || jackc_strcmp(reg, REG_TEMP) == 0
    || jackc_strcmp(reg, REG_THIS) == 0;
}

void vstack_push_mem(const vstack* s, const char* base, int word_offset) {
    int byte_offset = frame_offset_bytes(s->cfg, word_offset);
    if (is_section_not_stack_based(base)) {
        byte_offset = ABS(byte_offset);
    }
    asm_emit_lw(s->emit, REG_SCRATCH, byte_offset, base);
    vstack_push_reg(s, REG_SCRATCH);
}

void vstack_pop_mem(const vstack* s, const char* base, int word_offset) {
    int byte_offset = frame_offset_bytes(s->cfg, word_offset);
    if (is_section_not_stack_based(base)) {
        byte_offset = ABS(byte_offset);
    }
    vstack_pop_reg(s, REG_SCRATCH);
    asm_emit_sw(s->emit, REG_SCRATCH, byte_offset, base);
}

void vstack_alloc(const vstack* s, uint32_t n_words) {
    vstack_adjust(s, (int)n_words);
}

void vstack_dealloc(const vstack* s, uint32_t n_words) {
    vstack_adjust(s, -(int)n_words);
}
