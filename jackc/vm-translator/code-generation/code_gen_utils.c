#include "jackc_stdio.h"
#include "utils.h"
#include "vm-translator/constants.h"
#include <stdint.h>

void vm_code_gen_stack_alloc(int fd, int words) {
    jackc_fprintf(fd, "\taddi %s, %s, -%d\n", JACK_SP_REG, JACK_SP_REG, word_to_bytes(words));
}

void vm_code_gen_stack_dealloc(int fd, int words) {
    jackc_fprintf(fd, "\taddi %s, %s, %d\n", JACK_SP_REG, JACK_SP_REG, word_to_bytes(words));
}

void vm_code_gen_pop_idx(int fd, const char* dest_reg, int idx, bool deallocate) {
    jackc_fprintf(fd, "\tlw %s, %d(%s)\n", dest_reg, word_to_bytes(idx), JACK_SP_REG);
    if (deallocate) vm_code_gen_stack_dealloc(fd, 1);
}

void vm_code_gen_pop(int fd, const char* dest_reg, bool deallocate) {
    vm_code_gen_pop_idx(fd, dest_reg, 0, deallocate);
}


void vm_code_gen_push(int fd, const char* src_reg, bool allocate) {
    if (allocate) vm_code_gen_stack_alloc(fd, 1);
    jackc_fprintf(fd, "\tsw %s, 0(%s)\n", src_reg, JACK_SP_REG);
}

char* vm_segment_type_to_string(jackc_vm_segment_type segment_type) {
    switch (segment_type) {
        case SEGMENT_CONSTANT:
            return "constant";
        case SEGMENT_LOCAL:
            return "local";
        case SEGMENT_ARG:
            return "argument";
        case SEGMENT_THIS:
            return "this";
        case SEGMENT_THAT:
            return "that";
        case SEGMENT_TEMP:
            return "temp";
        case SEGMENT_POINTER:
            return "pointer";
        case SEGMENT_STATIC:
            return "static";
    }
    return "unknown";
}

int word_to_bytes(int word) {
    return word * 4;
}
