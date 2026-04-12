#ifndef JACKC_ASM_EMMITTER_H
#define JACKC_ASM_EMMITTER_H

#include "std/jackc_string.h"
#include <stdint.h>

typedef struct {
    int fd;
    bool emit_comments;
    uint32_t branch_label_idx;
    jackc_string branch_label;
} emitter;

typedef enum {
    C_UNARY_NEG,
    C_UNARY_NOT,
} vm_cmd_unary_arithm;

typedef enum {
    C_BINARY_ADD,
    C_BINARY_SUB,
    C_BINARY_MUL,
    C_BINARY_DIV,
    C_BINARY_AND,
    C_BINARY_OR,
    C_BINARY_LT,
    C_BINARY_GT,
    C_BINARY_EQ,
} vm_cmd_binary_arithm;

typedef enum {
    BRANCH_EQ,
    BRANCH_NE
} branch_cond;

typedef enum {
    SECTION_TEXT,
    SECTION_DATA,
} asm_section;

void asm_emit(const emitter* e, const char* content);

//============================
// Sections and directives
//============================
void asm_emit_section(const emitter* e, asm_section section);

void asm_emit_directive_space(const emitter* e, uint16_t bytes);

//============================
// Comments and label
//============================
void asm_emit_comment(const emitter* e, const char* fmt, ...);
void asm_emit_label(const emitter* e, const jackc_string* name);

//============================
// Register / Store operations
//============================
void asm_emit_lw(const emitter* e, const char* dst, int offset, const char* base);
void asm_emit_sw(const emitter* e, const char* src, int offset, const char* base);
void asm_emit_li(const emitter* e, const char* dst, int32_t imm);
void asm_emit_mv(const emitter* e, const char* dst, const char* src);
void asm_emit_la(const emitter* e, const char* dst, const jackc_string* label);

void asm_emit_add(const emitter* e, const char* dst, const char* s1, const char* s2);
void asm_emit_addi(const emitter* e, const char* dst, const char* s1, int32_t imm);
void asm_emit_sub(const emitter* e, const char* dst, const char* s1, const char* s2);
void asm_emit_slli(const emitter* e, const char* dst, const char* src, uint8_t shift);
void asm_emit_srli(const emitter* e, const char* dst, const char* src, uint8_t shift);

//============================
// Control flow
//============================
void asm_emit_call(const emitter* e, const jackc_string* target);
void asm_emit_j(const emitter* e, const jackc_string* target);
void asm_emit_branch(
    emitter* e,
    branch_cond cond,
    const char* r1,
    const char* r2,
    const jackc_string* label
);
void asm_emit_ret(const emitter* e);
void asm_emit_ecall(const emitter* e);

//============================
// Arithmetic
//============================
void asm_emit_unary_arithm(
    const emitter* e,
    vm_cmd_unary_arithm op,
    const char* dst,
    const char* src
);
void asm_emit_binary_arithm(
    const emitter* e,
    vm_cmd_binary_arithm op,
    const char* dst,
    const char* s1,
    const char* s2
);

#endif
