#ifndef JACKC_TYPE_MAPPERS_H
#define JACKC_TYPE_MAPPERS_H

#include "compiler/ast/ast.h"
#include "compiler/lexer/compiler_lexer.h"
#include "vm-translator/parser/vm_parser.h"

[[ nodiscard ]] char* token_type_to_str(jack_token_type token);

[[ nodiscard ]] jackc_string ast_type_to_str(const ast_type* ast_type);

[[ nodiscard ]] char* vm_segment_to_string(vm_segment seg);

#endif
