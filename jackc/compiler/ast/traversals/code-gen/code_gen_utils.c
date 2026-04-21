#include "code_gen_utils.h"
#include "compiler/ast/traversals/code-gen/code_gen_utils.h"
#include "compiler/ast/ast.h"
#include "compiler/ast/traversals/code-gen/std_utils.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/lexer/compiler_lexer.h"
#include "common/type_mappers.h"
#include "std/jackc_stdio.h"
#include "vm-translator/parser/vm_parser.h"

vm_segment vm_segment_from_variable_kind(jack_variable_kind kind) {
    switch (kind) {
        case VAR_STATIC_: return SEGMENT_STATIC;
        case VAR_FIELD: return SEGMENT_THIS;
        case VAR_LOCAL: return SEGMENT_LOCAL;
        case VAR_ARG: return SEGMENT_ARG;
    }

    return SEGMENT_ARG; // Make compiler happy;
}

void emit_push(FD fd, vm_segment seg, vm_index index) {
    jackc_fprintf(fd, "push %s %d\n", vm_segment_to_string(seg), index);
}

void emit_signed_const(FD fd, int32_t index) {
    jackc_fprintf(fd, "push %s %d\n", vm_segment_to_string(SEGMENT_CONSTANT), index);
}

void emit_pop(FD fd, vm_segment seg, vm_index index) {
    jackc_fprintf(fd, "pop %s %d\n", vm_segment_to_string(seg), index);
}

void emit_unary_arithmetic(FD fd, ast_unary_op op) {
    switch (op) {
        case UNARY_OP_NEG:
            jackc_fprintf(fd, "neg\n");
            break;
        case UNARY_OP_NOT:
            jackc_fprintf(fd, "not\n");
            break;
    }
}

void emit_binary_arithmetic_op(FD fd, ast_binary_op op) {
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

void emit_label(FD fd, const char* label, vm_index index) {
    jackc_fprintf(fd, "label %s_%d\n", label, index);
}

void emit_goto(FD fd, const char* label, vm_index index) {
    jackc_fprintf(fd, "goto %s_%d\n", label, index);
}

void emit_if_goto(FD fd, const char* label, vm_index index) {
    jackc_fprintf(fd, "if-goto %s_%d\n", label, index);
}

void emit_call(FD fd, const jackc_string* class_name, const jackc_string* subroutine_name, uint16_t n_args) {
    jackc_fprintf(
        fd,
        "call %.*s.%.*s %d\n",
        class_name->length, class_name->data,
        subroutine_name->length, subroutine_name->data,
        n_args
    );
}

void emit_function(FD fd, const jackc_string* class_name, const function_signature* sub_signature) {
    jackc_fprintf(
        fd,
        "function %.*s.%.*s %d\n",
        class_name->length, class_name->data,
        sub_signature->name.length, sub_signature->name.data,
        sub_signature->n_locals
    );
}

void emit_return(FD fd) {
    jackc_fprintf(fd, "return\n");
}

void emit_string_from_string_literal(FD fd, const jackc_string* string_literal) {
    emit_std_call(fd, (std_subroutine_call){
        .kind = STD_STRING_NEW,
        .string_new = {
            .length = string_literal->length
        }
    });
    emit_pop(fd, SEGMENT_TEMP, 0);
    for (uint32_t i = 0; i < string_literal->length; ++i) {
        emit_std_call(fd, (std_subroutine_call){
            .kind = STD_STRING_APPEND_CHAR,
            .string_append_char = {
                .c = (uint8_t)string_literal->data[i],
                .fetch_this_from_temp = true
            }
        });
    }
}
