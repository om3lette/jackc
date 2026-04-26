#ifndef JACKC_CODE_GEN_UTILS_H
#define JACKC_CODE_GEN_UTILS_H

#include "compiler/ast/ast.h"
#include "compiler/function-registry/function_registry.h"
#include "std/jackc_string.h"
#include "std/jackc_syscalls.h"
#include "vm-translator/parser/vm_parser.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define IF_TRUE_LABEL         "IF_TRUE"
#define IF_END_LABEL          "IF_END"
#define WHILE_CONDITION_LABEL "WHILE_CONDITION"
#define WHILE_END_LABEL       "WHILE_END"

typedef size_t vm_index;

[[ nodiscard ]] vm_segment vm_segment_from_variable_kind(jack_variable_kind kind);

void vm_emit_string_from_string_literal(FD fd, const jackc_string* string_literal);

void vm_emit_push(FD fd, vm_segment seg, vm_index index);

void vm_emit_signed_const(FD fd, int32_t index);

void vm_emit_pop(FD fd, vm_segment seg, vm_index index);

void vm_emit_unary_arithmetic(FD fd, ast_unary_op op);

void vm_emit_binary_arithmetic_op(FD fd, ast_binary_op op);

void vm_emit_label(FD fd, const char* label, vm_index index);

void vm_emit_goto(FD fd, const char* label, vm_index index);

void vm_emit_if_goto(FD fd, const char* label, vm_index index);

void vm_emit_call(FD fd, const jackc_string* class_name, const jackc_string* subroutine_name, uint16_t n_args);

void vm_emit_function(FD fd, const jackc_string* class_name, const function_signature* sub_signature);

void vm_emit_return(FD fd);

#endif
