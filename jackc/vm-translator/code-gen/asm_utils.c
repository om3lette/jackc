#include "vm-translator/code-gen/asm_emit.h"
#include "vm-translator/parser/vm_parser.h"

bool vm_cmd_to_unary_arithm(vm_cmd cmd, vm_cmd_unary_arithm* out) {
    switch (cmd) {
        case C_NEG:
            *out = C_UNARY_NEG;
            break;
        case C_NOT:
            *out = C_UNARY_NOT;
            break;
        default:
            return false;
    }
    return true;
}

bool vm_cmd_to_binary_arithm(vm_cmd cmd, vm_cmd_binary_arithm* out) {
    switch (cmd) {
        case C_ADD:
            *out = C_BINARY_ADD;
            break;
        case C_SUB:
            *out = C_BINARY_SUB;
            break;
        case C_MUL:
            *out = C_BINARY_MUL;
            break;
        case C_DIV:
            *out = C_BINARY_DIV;
            break;
        case C_EQ:
            *out = C_BINARY_EQ;
            break;
        case C_GT:
            *out = C_BINARY_GT;
            break;
        case C_LT:
            *out = C_BINARY_LT;
            break;
        case C_AND:
            *out = C_BINARY_AND;
            break;
        case C_OR:
            *out = C_BINARY_OR;
            break;
        default:
            return false;
    }
    return true;
}
