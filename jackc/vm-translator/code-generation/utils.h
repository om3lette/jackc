#ifndef JACKC_VM_TRANSLATOR_CODE_GENERATION_UTILS_H
#define JACKC_VM_TRANSLATOR_CODE_GENERATION_UTILS_H

#include "vm-translator/parser.h"

#define VM_CODE_GEN_HELP_COMMENT(fd, fmt, ...) \
    do { \
        jackc_fprintf(fd, "# "); \
        jackc_fprintf(fd, fmt, __VA_ARGS__); \
    } while(0)

#define VM_CODE_GEN_HELP_COMMENT_TAB(fd, fmt, ...) \
    do { \
        jackc_fprintf(fd, "\t# "); \
        jackc_fprintf(fd, fmt, __VA_ARGS__); \
    } while(0)

char* vm_code_gen_function_label(const jackc_string* name);

char* vm_segment_type_to_string(jackc_vm_segment_type segment_type);

int word_to_bytes(int word);

#endif
