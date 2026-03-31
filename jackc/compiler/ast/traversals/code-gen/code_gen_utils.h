#ifndef JACKC_CODE_GEN_UTILS_H
#define JACKC_CODE_GEN_UTILS_H

#include "compiler/ast/ast.h"
#include "compiler/function-registry/function_registry.h"
#include "jackc_string.h"
#include "vm-translator/parser/vm_parser.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define IF_TRUE_LABEL         "IF_TRUE"
#define IF_END_LABEL          "IF_END"
#define WHILE_CONDITION_LABEL "WHILE_CONDITION"
#define WHILE_END_LABEL       "WHILE_END"

typedef size_t vm_index;

void emit_string_from_string_literal(int fd, const jackc_string* string_literal);

[[ nodiscard ]] jackc_vm_segment_type vm_segment_from_variable_kind(jack_variable_kind kind);

[[ nodiscard ]] char* vm_segment_to_string(jackc_vm_segment_type seg);

void emit_push(int fd, jackc_vm_segment_type seg, vm_index index);

void emit_signed_const(int fd, int32_t index);

void emit_pop(int fd, jackc_vm_segment_type seg, vm_index index);

void emit_unary_arithmetic(int fd, ast_unary_op op);

void emit_binary_arithmetic_op(int fd, ast_binary_op op);

void emit_label(int fd, const char* label, vm_index index);

void emit_goto(int fd, const char* label, vm_index index);

void emit_if_goto(int fd, const char* label, vm_index index);

void emit_call(int fd, const jackc_string* class_name, const jackc_string* subroutine_name, uint16_t n_args);

void emit_function(int fd, const jackc_string* class_name, const function_signature* sub_signature);

void emit_return(int fd);

#endif
