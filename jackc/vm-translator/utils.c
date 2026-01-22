#include "vm-translator/utils.h"

bool jackc_vm_cmd_is_arithmetic(jackc_vm_cmd_type cmd_type) {
    switch (cmd_type) {
        case C_ADD:
        case C_SUB:
        case C_NEG:
        case C_EQ:
        case C_GT:
        case C_LT:
        case C_AND:
        case C_OR:
        case C_NOT:
            return true;
        default:
            return false;
    }
}

char* jackc_cmd_type_to_string(jackc_vm_cmd_type cmd_type) {
    switch (cmd_type) {
        case C_ADD:
            return "add";
        case C_SUB:
            return "sub";
        case C_NEG:
            return "neg";
        case C_EQ:
            return "eq";
        case C_GT:
            return "gt";
        case C_LT:
            return "lt";
        case C_AND:
            return "and";
        case C_OR:
            return "or";
        case C_NOT:
            return "not";
        case C_PUSH:
            return "push";
        case C_POP:
            return "pop";
        case C_LABEL:
            return "label";
        case C_GOTO:
            return "goto";
        case C_IF_GOTO:
            return "if-goto";
        case C_FUNCTION:
            return "function";
        case C_RETURN:
            return "return";
        case C_CALL:
            return "call";
        default:
            return "unknown";
    }
}
