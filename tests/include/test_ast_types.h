#ifndef TEST_AST_TYPES_H
#define TEST_AST_TYPES_H

#include "compiler/ast/ast.h" // IWYU pragma: keep
#include "std/jackc_string.h"

#define T_VOID (ast_type){ .kind = TYPE_VOID, .class_name = jackc_string_empty() }
#define T_INT (ast_type){ .kind = TYPE_INT, .class_name = jackc_string_empty() }
#define T_CHAR (ast_type){ .kind = TYPE_CHAR, .class_name = jackc_string_empty() }
#define T_BOOLEAN (ast_type){ .kind = TYPE_BOOLEAN, .class_name = jackc_string_empty() }
#define T_CLASS(_class_name) (ast_type){ .kind = TYPE_CLASS, .class_name = jackc_string_from_str(_class_name) }

#endif
