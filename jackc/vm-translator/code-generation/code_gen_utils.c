#include "jackc_stdio.h"
#include "utils.h"

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

/**
 * @todo: add filename/index to a label
 * @todo: genererate static vars definitions at the end if the file
 */
char* vm_code_gen_generate_static_name(uint32_t index) {
    static char static_label_buffer[32];
    jackc_sprintf(static_label_buffer, "STATIC_%d", index);
    return static_label_buffer;
}

int word_to_bytes(int word) {
    return word * 4;
}
