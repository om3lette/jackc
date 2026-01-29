#ifndef JACKC_VM_TRANSLATOR_CODE_GENERATION_UTILS_H
#define JACKC_VM_TRANSLATOR_CODE_GENERATION_UTILS_H

#include "vm-translator/parser.h"

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

int word_to_bytes(int word);

void vm_code_gen_stack_alloc(int fd, int words);

void vm_code_gen_stack_dealloc(int fd, int words);

void vm_code_gen_pop(int fd, const char* dest_reg, bool deallocate);

void vm_code_gen_pop_idx(int fd, const char* dest_reg, int idx, bool deallocate);

void vm_code_gen_push(int fd, const char* dest_reg, bool allocate);

#endif
