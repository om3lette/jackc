#include "asm_emit.h"
#include "std/jackc_stdio.h"
#include "std/jackc_string.h"
#include "vm-translator/code-gen/regs.h"
#include <stdarg.h>

void asm_emit(const emitter* e, const char* content) {
    jackc_fprintf(e->fd, "%s", content);
}

//============================
// Sections and directives
//============================
static const char* section_strs[] = {
    [SECTION_DATA] = ".data",
    [SECTION_TEXT] = ".text",
};
void asm_emit_section(const emitter* e, asm_section section) {
    const char* section_str = section_strs[section];
    jackc_fprintf(e->fd, "%s\n", section_str);
}

void asm_emit_directive_space(const emitter* e, uint16_t bytes) {
    jackc_fprintf(e->fd, ".space %d\n", bytes);
}


//============================
// Comments and label
//============================
void asm_emit_comment(const emitter* e, const char* fmt, ...) {
    jackc_fprintf(e->fd, "\t# ");
    va_list args;
    va_start(args, fmt);
    jackc_vfprintf(e->fd, fmt, args);
    va_end(args);
    jackc_fprintf(e->fd, "\n");
}

void asm_emit_label(const emitter* e, const jackc_string* name) {
    jackc_fprintf(e->fd, "%.*s:\n", name->length, name->data);
}


//============================
// Register / Store operations
//============================
void asm_emit_lw(const emitter* e, const char* dst, int offset, const char* base) {
    jackc_fprintf(e->fd, "\tlw %s, %d(%s)\n", dst, offset, base);
}

void asm_emit_sw(const emitter* e, const char* src, int offset, const char* base) {
    jackc_fprintf(e->fd, "\tsw %s, %d(%s)\n", src, offset, base);
}
void asm_emit_li(const emitter* e, const char* dst, int32_t imm) {
    jackc_fprintf(e->fd, "\tli %s, %d\n", dst, imm);
}
void asm_emit_mv(const emitter* e, const char* dst, const char* src) {
    asm_emit_add(e, dst, src, REG_ZERO);
}
void asm_emit_la(const emitter* e, const char* dst, const jackc_string* label) {
    jackc_fprintf(e->fd, "\tla %s, %.*s\n", dst, label->length, label->data);
}

void asm_emit_add(const emitter* e, const char* dst, const char* s1, const char* s2) {
    jackc_fprintf(e->fd, "\tadd %s, %s, %s\n", dst, s1, s2);
}
void asm_emit_addi(const emitter* e, const char* dst, const char* s1, int32_t imm) {
    jackc_fprintf(e->fd, "\taddi %s, %s, %d\n", dst, s1, imm);
}
void asm_emit_sub(const emitter* e, const char* dst, const char* s1, const char* s2) {
    jackc_fprintf(e->fd, "\tadd %s, %s, %s\n", dst, s1, s2);
}
void asm_emit_slli(const emitter* e, const char* dst, const char* src, uint8_t shift) {
    jackc_fprintf(e->fd, "\tslli %s, %s, %d\n", dst, src, shift);
}
void asm_emit_srli(const emitter* e, const char* dst, const char* src, uint8_t shift) {
    jackc_fprintf(e->fd, "\tsrli %s, %s, %d\n", dst, src, shift);
}

//============================
// Control flow
//============================
void asm_emit_call(const emitter* e, const jackc_string* target) {
    jackc_fprintf(e->fd, "\tcall %.*s\n", target->length, target->data);
}
void asm_emit_j(const emitter* e, const jackc_string* target) {
    jackc_fprintf(e->fd, "\tj %.*s\n", target->length, target->data);
}

// TODO: Proper API
void asm_emit_ecall(const emitter* e) {
    jackc_fprintf(e->fd, "\tecall\n");
}

static const char* branch_cond_strs[] = {
    [BRANCH_EQ]  = "bne",
    [BRANCH_NE]  = "beq",
};
void asm_emit_branch(
    emitter* e,
    branch_cond cond,
    const char* r1,
    const char* r2,
    const jackc_string* label
) {
    asm_emit_comment(e, "RISC-V branches only support jumps which fit into 12 bits, while j can handle 20 bits");
    jackc_fprintf(
        e->fd,
        "\t%s %s, %s, %s_%d\n",
        branch_cond_strs[cond], r1, r2,
        e->branch_label.data, e->branch_label_idx
    );
    asm_emit_j(e, label);
    jackc_fprintf(
        e->fd,
        "%s_%d:\n",
        e->branch_label.data, e->branch_label_idx
    );

    ++e->branch_label_idx;
}
void asm_emit_ret(const emitter* e) {
    jackc_fprintf(e->fd, "\tret\n");
}

//============================
// Arithmetic
//============================
static const char* unary_aritm_strs[] = {
    [C_UNARY_NEG]  = "neg",
    [C_UNARY_NOT]  = "seqz",
};
void asm_emit_unary_arithm(
    const emitter* e,
    vm_cmd_unary_arithm op,
    const char* dst,
    const char* src
) {
    const char* op_str = unary_aritm_strs[op];
    jackc_fprintf(e->fd, "\t%s %s, %s\n", op_str, dst, src);
}

// C_BINARY_EQ is handled separately
static const char* binary_aritm_strs[] = {
    [C_BINARY_ADD] = "add",
    [C_BINARY_SUB] = "sub",
    [C_BINARY_MUL] = "mul",
    [C_BINARY_DIV] = "div",
    [C_BINARY_AND] = "and",
    [C_BINARY_OR]  = "or",
    [C_BINARY_LT]  = "slt",
    [C_BINARY_GT]  = "sgt",
    [C_BINARY_EQ]  = "seqz"
};
void asm_emit_binary_arithm(
    const emitter* e,
    vm_cmd_binary_arithm op,
    const char* dst,
    const char* s1,
    const char* s2
) {
    const char* op_str = binary_aritm_strs[op];
    if (op == C_BINARY_EQ) {
        jackc_fprintf(e->fd, "\txor %s, %s, %s\n", dst, s1, s2);
        jackc_fprintf(e->fd, "\t%s, %s, %s\n", op_str, dst, dst);
        return;
    }
    jackc_fprintf(e->fd, "\t%s %s, %s, %s\n", op_str, dst, s1, s2);
}
