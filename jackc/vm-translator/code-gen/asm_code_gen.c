#include "vm-translator/code-gen/asm_code_gen.h"
#include "common/common.h"
#include "core/allocators/allocators.h"
#include "core/asserts/jackc_assert.h"
#include "core/logging/logger.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"
#include "vm-translator/code-gen/asm_emit.h"
#include "vm-translator/code-gen/asm_utils.h"
#include "vm-translator/code-gen/regs.h"
#include "vm-translator/code-gen/stack_frame.h"
#include "vm-translator/code-gen/virtual_stack.h"
#include "vm-translator/constants.h"
#include "vm-translator/parser/vm_parser.h"
#include "vm-translator/parser/vm_parser_utils.h"

asm_context* asm_context_init(int fd, const jackc_config* config, Allocator* allocator) {
    asm_context* ctx = allocator->alloc(sizeof(asm_context), allocator->context);
    ctx->static_idx = 0;
    ctx->n_locals = 0;
    ctx->static_label = jackc_string_from_str("_JACKC_STATIC");
    ctx->temp_label = jackc_string_from_str("_JACKC_TMP");
    ctx->e = (emitter){
        .fd = fd
    };
    ctx->cfg = (frame_config){
        .word_size = 4,
        .n_regs_saved = FRAME_SLOT_COUNT,
        .grows_up = config->is_stack_growing_upwards,
        .stack_size = config->upward_stack_size
    };
    ctx->s = (vstack){
        .emit = &ctx->e,
        .cfg = &ctx->cfg,
        .sp_reg = REG_SP
    };

    return ctx;
}

static void codegen_push(asm_context* ctx, vm_segment seg, int idx) {
    asm_emit_comment(&ctx->e, "push %s %d", vm_segment_to_string(seg), idx);

    // Index is never negative and is 0 indexed
    // To distinguish between "no static vars" and "a single 0 indexed static var"
    // 1 is added to make it 1 indexed
    if (seg == SEGMENT_STATIC)
        ctx->static_idx = MAX(ctx->static_idx, (uint32_t)idx + 1);

    const char* base_reg = REG_SP;
    switch (seg) {
        case SEGMENT_POINTER:
            vstack_push_reg(&ctx->s, idx == POINTER_THIS ? REG_THIS : REG_THAT);
            return;
        case SEGMENT_CONSTANT:
            vstack_push_imm(&ctx->s, idx);
            return;
        case SEGMENT_STATIC: base_reg = REG_STATIC; break;
        case SEGMENT_ARG: base_reg = REG_ARG; break;
        case SEGMENT_LOCAL: base_reg = REG_LOCAL; break;
        case SEGMENT_THIS: base_reg = REG_THIS; break;
        case SEGMENT_THAT: base_reg = REG_THAT; break;
        case SEGMENT_TEMP: base_reg = REG_TEMP; break;
    }
    vstack_push_mem(&ctx->s, base_reg, idx);
}
static void codegen_pop(asm_context* ctx, vm_segment seg, int idx) {
    asm_emit_comment(&ctx->e, "pop %s %d", vm_segment_to_string(seg), idx);

    // Index is never negative and is 0 indexed
    // To distinguish between "no static vars" and "a single 0 indexed static var"
    // 1 is added to make it 1 indexed
    if (seg == SEGMENT_STATIC)
        ctx->static_idx = MAX(ctx->static_idx, (uint32_t)idx + 1);

    const char* base_reg = REG_SP;
    switch (seg) {
        case SEGMENT_POINTER:
            vstack_pop_reg(&ctx->s, idx == POINTER_THIS ? REG_THIS : REG_THAT);
            return;
        case SEGMENT_CONSTANT:
            // Parser should have filtered out this case
            jackc_assert(false && "Invalid operation.");
            return;
        case SEGMENT_STATIC: base_reg = REG_STATIC; break;
        case SEGMENT_ARG: base_reg = REG_ARG; break;
        case SEGMENT_LOCAL: base_reg = REG_LOCAL; break;
        case SEGMENT_THIS: base_reg = REG_THIS; break;
        case SEGMENT_THAT: base_reg = REG_THAT; break;
        case SEGMENT_TEMP: base_reg = REG_TEMP; break;
    }
    vstack_pop_mem(&ctx->s, base_reg, idx);
}

static void codegen_alu(asm_context* ctx, vm_cmd cmd) {
    asm_emit_comment(&ctx->e, "%s", vm_cmd_type_to_string(cmd));

    vm_cmd_unary_arithm unary_cmd;
    if (vm_cmd_to_unary_arithm(cmd, &unary_cmd)) {
        vstack_peek_reg(&ctx->s, REG_OP1, 0);
        asm_emit_unary_arithm(&ctx->e, unary_cmd, REG_RES, REG_OP1);
        vstack_poke_reg(&ctx->s, REG_RES, 0);
        return;
    }

    vm_cmd_binary_arithm binary_cmd;
    if (vm_cmd_to_binary_arithm(cmd, &binary_cmd)) {
        vstack_pop_reg(&ctx->s, REG_OP2);
        vstack_peek_reg(&ctx->s, REG_OP1, 0);
        asm_emit_binary_arithm(&ctx->e, binary_cmd, REG_RES, REG_OP1, REG_OP2);
        vstack_poke_reg(&ctx->s, REG_RES, 0);
        return;
    }

    // Invalid dispatch -> code error
    LOG_FATAL("Invalid command passed to codegen_alu: %d\n", cmd);
    jackc_assert(false);
}

static void codegen_label(asm_context* ctx, const jackc_string* name) {
    asm_emit_label(&ctx->e, name);
}
static void codegen_goto(asm_context* ctx, const jackc_string* label) {
    asm_emit_j(&ctx->e, label);
}
static void codegen_if_goto(asm_context* ctx, const jackc_string* label) {
    vstack_pop_reg(&ctx->s, REG_RES);
    asm_emit_branch(&ctx->e, BRANCH_NE, REG_RES, REG_ZERO, label);
}
static void codegen_call(asm_context* ctx, const jackc_string* name, int n_args) {
    asm_emit_comment(&ctx->e, "Save registers");
    vstack_alloc(&ctx->s, FRAME_SLOT_COUNT);
    vstack_poke_reg(&ctx->s, REG_LOCAL, FRAME_SLOT_LOCAL);
    vstack_poke_reg(&ctx->s, REG_ARG, FRAME_SLOT_ARG);
    vstack_poke_reg(&ctx->s, REG_THAT, FRAME_SLOT_THAT);
    vstack_poke_reg(&ctx->s, REG_THIS, FRAME_SLOT_THIS);
    vstack_poke_reg(&ctx->s, REG_RET_ADDR, FRAME_SLOT_RET_ADDR);
    if (n_args > 0) {
        asm_emit_comment(&ctx->e, "Set ARG pointer");
        asm_emit_addi(&ctx->e, REG_ARG, REG_SP, frame_offset_bytes(&ctx->cfg, FRAME_SLOT_COUNT));
    }

    asm_emit_call(&ctx->e, name);

    asm_emit_comment(&ctx->e, "Restore registers");
    vstack_peek_reg(&ctx->s, REG_LOCAL, FRAME_SLOT_LOCAL);
    vstack_peek_reg(&ctx->s, REG_ARG, FRAME_SLOT_ARG);
    vstack_peek_reg(&ctx->s, REG_THAT, FRAME_SLOT_THAT);
    vstack_peek_reg(&ctx->s, REG_THIS, FRAME_SLOT_THIS);
    vstack_peek_reg(&ctx->s, REG_RET_ADDR, FRAME_SLOT_RET_ADDR);
    // Reserve space for the return value by not deallocating one of the frame slots
    vstack_dealloc(&ctx->s, FRAME_SLOT_COUNT + (uint32_t)n_args - 1);

    asm_emit_comment(&ctx->e, "Put return value on the stack");
    vstack_poke_reg(&ctx->s, REG_RET, 0);
}
static void codegen_return(asm_context* ctx) {
    asm_emit_comment(&ctx->e, "Pop return value from the stack");
    vstack_peek_reg(&ctx->s, REG_RET, 0);

    asm_emit_comment(&ctx->e, "Restore stack pointer");
    asm_emit_addi(&ctx->e, REG_SP, REG_LOCAL, frame_offset_bytes(&ctx->cfg, ctx->n_locals));

    asm_emit_ret(&ctx->e);
}

static void codegen_function(asm_context* ctx, const jackc_string* name, int n_locals) {
    asm_emit_label(&ctx->e, name);

    ctx->n_locals = (uint16_t)n_locals;
    if (n_locals != 0) {
        asm_emit_comment(&ctx->e, "Allocate space for local variables and setup local ptr");
        vstack_alloc(&ctx->s, (uint32_t)n_locals);
    } else {
        asm_emit_comment(&ctx->e, "Initialize local ptr as a frame anchor");
    }
    asm_emit_mv(&ctx->e, REG_LOCAL, REG_SP);
}

void asm_code_gen_process_line(asm_context* ctx, const vm_parser* parser) {
    const vm_line* line = &parser->current;
    switch (line->cmd) {
        case C_PUSH:     codegen_push(ctx, line->arg1.segment, line->arg2.value);  break;
        case C_POP:      codegen_pop(ctx, line->arg1.segment, line->arg2.value);   break;
        case C_ADD:
        case C_SUB:
        case C_AND:
        case C_OR:
        case C_MUL:
        case C_DIV:
        case C_NEG:
        case C_NOT:
        case C_EQ:
        case C_GT:
        case C_LT:       codegen_alu(ctx, line->cmd);                              break;
        case C_LABEL:    codegen_label(ctx, &line->arg1.str);                      break;
        case C_GOTO:     codegen_goto(ctx, &line->arg1.str);                       break;
        case C_IF_GOTO:  codegen_if_goto(ctx, &line->arg1.str);                    break;
        case C_CALL:     codegen_call(ctx, &line->arg1.str, line->arg2.value);     break;
        case C_RETURN:   codegen_return(ctx);                                      break;
        case C_FUNCTION: codegen_function(ctx, &line->arg1.str, line->arg2.value); break;
    }
}

void asm_code_gen_bootstrap(const asm_context* ctx) {
    asm_emit_section(&ctx->e, SECTION_TEXT);
    asm_emit_label(&ctx->e, &jackc_string_from_str("_start"));

    // Static/Temp labels will be generated by `finalize`
    // As number of static variables used is calculated during emitting
    asm_emit_comment(&ctx->e, "Initialize STATIC pointer");
    asm_emit_la(&ctx->e, REG_STATIC, &ctx->static_label);
    asm_emit_comment(&ctx->e, "Initialize TEMP pointer");
    asm_emit_la(&ctx->e, REG_TEMP, &ctx->temp_label);

    const frame_config* cfg = &ctx->cfg;
    if (cfg->grows_up) {
        asm_emit_comment(&ctx->e, "Allocate space for the stack (grows upwards)");

        // This is a special case when "allocation" is in the opposite direction of actual stack growth
        int n_words = (int)(cfg->stack_size + (cfg->word_size - 1)) / cfg->word_size;
        int32_t bytes_to_alloc = frame_offset_bytes(&ctx->cfg, (int)n_words);
        asm_emit_li(&ctx->e, REG_SCRATCH, bytes_to_alloc);
        asm_emit_add(&ctx->e, REG_RES, REG_RES, REG_SCRATCH);
    }

    asm_emit_comment(&ctx->e, "Prepare argc, argv for Main.main");
    vstack_alloc(&ctx->s, 2);

    asm_emit_sw(&ctx->e, "a1", frame_offset_bytes(cfg, 1), REG_SP);
    asm_emit_sw(&ctx->e, "a0", 0, REG_SP);

    asm_emit_addi(&ctx->e, REG_ARG, REG_SP, 0);

    // TODO: Generalize
    asm_emit_label(&ctx->e, &jackc_string_from_str("Sys.init"));
    asm_emit_call(&ctx->e, &jackc_string_from_str("Memory.init"));
    asm_emit_call(&ctx->e, &jackc_string_from_str("Main.main"));

    #ifndef NDEBUG
        asm_emit_comment(&ctx->e, "Print return value");
        asm_emit_li(&ctx->e, "a7", 1);
        asm_emit_ecall(&ctx->e);
    #endif

    asm_emit_comment(&ctx->e, "Exit with return value as exit code");
    asm_emit_li(&ctx->e, "a7", 93);
    asm_emit_ecall(&ctx->e);
}

void asm_code_gen_finalize(const asm_context* ctx) {
    asm_emit_section(&ctx->e, SECTION_DATA);
    asm_emit_label(&ctx->e, &ctx->static_label);
    asm_emit_directive_space(&ctx->e, (uint16_t)(ctx->static_idx * ctx->cfg.word_size));

    asm_emit_label(&ctx->e, &ctx->temp_label);
    asm_emit_directive_space(&ctx->e, (uint16_t)(8 * ctx->cfg.word_size));
}
