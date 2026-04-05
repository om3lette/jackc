#ifndef JACKC_VM_CODE_GENERATOR_H
#define JACKC_VM_CODE_GENERATOR_H


#include "core/allocators/allocators.h"
#include "core/config.h"
#include "vm-translator/code-gen/stack_frame.h"
#include "vm-translator/code-gen/virtual_stack.h"
#include "vm-translator/parser/vm_parser.h"
#include <stddef.h>

typedef struct {
    emitter e;
    vstack s;
    frame_config cfg;

    uint32_t static_idx;
    uint16_t n_locals;

    jackc_string static_label;
    jackc_string temp_label;
} asm_context;

asm_context* asm_context_init(int fd, const jackc_config* config, Allocator* allocator);

void asm_code_gen_bootstrap(const asm_context* ctx);

void asm_code_gen_process_line(asm_context* ctx, const vm_parser* parser);

void asm_code_gen_finalize(const asm_context* ctx);

#endif
