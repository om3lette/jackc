#ifndef JACKC_VM_TRANSLATOR_CODE_GEN_UTILS_H
#define JACKC_VM_TRANSLATOR_CODE_GEN_UTILS_H

#include "common/config.h"
#include "vm-translator/parser/vm_parser.h"

#define VM_CODE_GEN_HELP_COMMENT(fd, fmt, ...) \
    do { \
        jackc_fprintf(fd, "# "); \
        jackc_fprintf(fd, fmt __VA_OPT__(,) __VA_ARGS__); \
    } while(0)

#define VM_CODE_GEN_HELP_COMMENT_TAB(fd, fmt, ...) \
    do { \
        jackc_fprintf(fd, "\t# "); \
        jackc_fprintf(fd, fmt __VA_OPT__(,) __VA_ARGS__); \
    } while(0)

char* vm_segment_type_to_string(jackc_vm_segment_type segment_type);

void vm_code_gen_stack_alloc(int fd, int words, const jackc_config_t* ctx);

void vm_code_gen_stack_dealloc(int fd, int words, const jackc_config_t* ctx);

void vm_code_gen_pop(int fd, const char* dest_reg, bool deallocate, const jackc_config_t* ctx);

void vm_code_gen_pop_idx(int fd, const char* dest_reg, int idx, bool deallocate, const jackc_config_t* ctx);

void vm_code_gen_push(int fd, const char* dest_reg, bool allocate, const jackc_config_t* ctx);

#endif
