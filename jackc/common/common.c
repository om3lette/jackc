#include "vm-translator/parser/vm_parser.h"

char* vm_segment_to_string(vm_segment seg) {
    switch (seg) {
        case SEGMENT_THIS: return "this";
        case SEGMENT_THAT: return "that";
        case SEGMENT_ARG: return "argument";
        case SEGMENT_LOCAL: return "local";
        case SEGMENT_CONSTANT: return "constant";
        case SEGMENT_STATIC: return "static";
        case SEGMENT_TEMP: return "temp";
        case SEGMENT_POINTER: return "pointer";
    }

    return "unknown";
}
