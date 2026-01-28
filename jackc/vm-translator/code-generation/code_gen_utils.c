#include "common/jackc_assert.h"
#include "jackc_stdio.h"
#include "utils.h"
#include "vm-translator/constants.h"

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
 */
char* vm_code_gen_generate_static_name(uint32_t index) {
    static char static_label_buffer[32];
    jackc_sprintf(static_label_buffer, "STATIC_%d", index);
    return static_label_buffer;
}

char* vm_code_gen_function_label(const jackc_string* name) {
    // It is assumed that the parser has already validated the function name
    jackc_assert(
        name != NULL
        && name->length != 0 && name->length < MAX_FUNCTION_NAME_LENGTH
        && name->data != NULL
        && "Invalid function name input."
    );
    static char label_function_buffer[MAX_FUNCTION_NAME_LENGTH + 32];
    jackc_sprintf(label_function_buffer, "%.*s", name->length, name->data);
    label_function_buffer[name->length] = '\0';
    return label_function_buffer;
}

int word_to_bytes(int word) {
    return word * 4;
}
