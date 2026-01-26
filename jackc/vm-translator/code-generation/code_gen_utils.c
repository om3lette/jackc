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
