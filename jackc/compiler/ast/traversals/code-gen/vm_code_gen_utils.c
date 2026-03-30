// #include "vm_code_gen_utils.h"
#include "compiler/ast/traversals/code-gen/vm_code_gen_utils.h"
#include "compiler/ast/ast.h"
#include "compiler/ast/traversals/code-gen/std_utils.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/lexer/compiler_lexer.h"
#include "jackc_stdio.h"
#include "vm-translator/parser/vm_parser.h"

jackc_vm_segment_type vm_segment_from_variable_kind(jack_variable_kind kind) {
    switch (kind) {
        case VAR_STATIC: return SEGMENT_STATIC;
        case VAR_FIELD: return SEGMENT_THIS;
        case VAR_LOCAL: return SEGMENT_LOCAL;
        case VAR_ARG: return SEGMENT_ARG;
    }

    return SEGMENT_ARG; // Make compiler happy;
}

char* vm_segment_to_string(jackc_vm_segment_type seg) {
    switch (seg) {
        case SEGMENT_THIS: return "this";
        case SEGMENT_THAT: return "that";
        case SEGMENT_ARG: return "argument";
        case SEGMENT_LOCAL: return "local";
        case SEGMENT_CONSTANT: return "constant";
        case SEGMENT_STATIC: return "static";
        case SEGMENT_TEMP: return "temp";
        case SEGMENT_POINTER: return "pointer";
        default: return nullptr;
    }
}

void emit_push(int fd, jackc_vm_segment_type seg, vm_index index) {
    jackc_fprintf(fd, "push %s %d\n", vm_segment_to_string(seg), index);
}

void emit_signed_const(int fd, int32_t index) {
    jackc_fprintf(fd, "push %s %d\n", vm_segment_to_string(SEGMENT_CONSTANT), index);
}

void emit_pop(int fd, jackc_vm_segment_type seg, vm_index index) {
    jackc_fprintf(fd, "pop %s %d\n", vm_segment_to_string(seg), index);
}

void emit_unary_arithmetic(int fd, ast_unary_op op) {
    switch (op) {
        case UNARY_OP_NEG:
            jackc_fprintf(fd, "neg\n");
            break;
        case UNARY_OP_NOT:
            jackc_fprintf(fd, "not\n");
            break;
    }
}

void emit_binary_arithmetic_op(int fd, ast_binary_op op) {
    switch (op) {
        case BINARY_OP_ADD:
            jackc_fprintf(fd, "add\n");
            break;
        case BINARY_OP_SUB:
            jackc_fprintf(fd, "sub\n");
            break;
        case BINARY_OP_MUL:
            jackc_fprintf(fd, "mul\n");
            break;
        case BINARY_OP_DIV:
            jackc_fprintf(fd, "div\n");
            break;
        case BINARY_OP_AND:
            jackc_fprintf(fd, "and\n");
            break;
        case BINARY_OP_OR:
            jackc_fprintf(fd, "or\n");
            break;
        case BINARY_OP_LT:
            jackc_fprintf(fd, "lt\n");
            break;
        case BINARY_OP_GT:
            jackc_fprintf(fd, "gt\n");
            break;
        case BINARY_OP_EQ:
            jackc_fprintf(fd, "eq\n");
            break;
    }
}

void emit_label(int fd, const char* label, vm_index index) {
    jackc_fprintf(fd, "label %s_%d\n", label, index);
}

void emit_goto(int fd, const char* label, vm_index index) {
    jackc_fprintf(fd, "goto %s_%d\n", label, index);
}

void emit_if_goto(int fd, const char* label, vm_index index) {
    jackc_fprintf(fd, "if-goto %s_%d\n", label, index);
}

void emit_call(int fd, const jackc_string* class_name, const jackc_string* subroutine_name, uint16_t n_args) {
    jackc_fprintf(
        fd,
        "call %.*s.%.*s %d\n",
        class_name->length, class_name->data,
        subroutine_name->length, subroutine_name->data,
        n_args
    );
}

void emit_function(int fd, const jackc_string* class_name, const function_signature* sub_signature) {
    jackc_fprintf(
        fd,
        "function %.*s.%.*s %d\n",
        class_name->length, class_name->data,
        sub_signature->name.length, sub_signature->name.data,
        sub_signature->n_locals
    );
}

void emit_return(int fd) {
    jackc_fprintf(fd, "return\n");
}

void emit_string_from_string_literal(int fd, const jackc_string* string_literal) {
    emit_std_call(fd, (std_subroutine_call){
        .kind = STD_STRING_NEW,
        .string_new = {
            .length = string_literal->length
        }
    });
    for (uint32_t i = 0; i < string_literal->length; ++i) {
        emit_std_call(fd, (std_subroutine_call){
            .kind = STD_STRING_APPEND_CHAR,
            .string_append_char = {
                .c = (uint8_t)string_literal->data[i]
            }
        });
    }
}
