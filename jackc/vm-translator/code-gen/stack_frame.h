#ifndef JACKC_STACK_FRAME_H
#define JACKC_STACK_FRAME_H

#include <stdint.h>

typedef struct {
    uint8_t word_size;    // Bytes per word (4 for rv32)
    uint8_t word_bits;    // Bits required to store one word
    uint8_t n_regs_saved; // How many regs saved on call
    bool grows_up;        // Stack growth direction
    uint32_t stack_size;  // Stack size in bytes (valid only if `grows_up` is set to `true`)
} frame_config;

/**
 * Example stack layout for down growing stack:
 * [sp+32] argument 1     |
 * [sp+28] argument 0     | <- ARG ptr
 * [sp+24] OLD_LCL        |
 * [sp+20] OLD_ARG        |
 * [sp+16] OLD_THAT       |
 * [sp+12] OLD_THIS       |
 * [sp+ 8] return address |
 * [sp+ 4] local arg 1    |
 * [sp+ 0] local arg 0    | <- LCL ptr
 *
*/

typedef enum {
    FRAME_SLOT_RET_ADDR  = 0,
    FRAME_SLOT_THIS,
    FRAME_SLOT_THAT,
    FRAME_SLOT_ARG,
    FRAME_SLOT_LOCAL,

    FRAME_SLOT_COUNT
} frame_slot;

[[ nodiscard ]] int frame_offset_bytes(const frame_config* cfg, int word_idx);

#endif
