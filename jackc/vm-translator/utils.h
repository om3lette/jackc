#ifndef JACKC_VM_TRANSLATOR_UTILS_H
#define JACKC_VM_TRANSLATOR_UTILS_H

#include "vm-translator/parser.h"

char* jackc_cmd_type_to_string(jackc_vm_cmd_type cmd_type);

bool jackc_vm_cmd_is_arithmetic(jackc_vm_cmd_type cmd_type);

#endif
