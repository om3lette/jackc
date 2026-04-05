#ifndef JACKC_ASM_UTILS_H
#define JACKC_ASM_UTILS_H

#include "vm-translator/code-gen/asm_emit.h"
#include "vm-translator/parser/vm_parser.h"

/**
 * Returns true if the given command type is an arithmetic command.
 *
 * @param cmd_type The command type to check.
 */
[[ nodiscard ]] bool vm_cmd_to_unary_arithm(vm_cmd cmd, vm_cmd_unary_arithm* out);

[[ nodiscard ]] bool vm_cmd_to_binary_arithm(vm_cmd cmd, vm_cmd_binary_arithm* out);

#endif
