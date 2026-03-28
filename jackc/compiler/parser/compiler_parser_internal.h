#ifndef JACKC_COMPILER_PARSER_INTERNAL_H
#define JACKC_COMPILER_PARSER_INTERNAL_H

#include "compiler/ast/ast.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include <stdint.h>

typedef uint8_t binding_power;

#define PREFIX_BP (binding_power)255

#define SYNC_IF_PANIC(parser) do { \
    if (is_panic_mode(parser)) \
        jack_parser_sync(parser); \
} while (0)

#define RETURN_IF_PANIC(parser) do { \
    if (is_panic_mode(parser)) \
        return nullptr; \
} while (0)

#define EXPECT_LEFT_PAREN(parser) do { \
    jack_parser_expect(parser, '('); \
    RETURN_IF_PANIC(parser); \
} while (0)

#define EXPECT_RIGHT_PAREN(parser) do { \
    jack_parser_expect(parser, ')'); \
    RETURN_IF_PANIC(parser); \
} while (0)

#define EXPECT_LEFT_CURLY_BRACE(parser) do { \
    jack_parser_expect(parser, '{'); \
    RETURN_IF_PANIC(parser); \
} while (0)

#define EXPECT_RIGHT_CURLY_BRACE(parser) do { \
    jack_parser_expect(parser, '}'); \
    RETURN_IF_PANIC(parser); \
} while (0)

#define EXPECT_SEMICOLON(parser) do { \
    jack_parser_expect(parser, ';'); \
    RETURN_IF_PANIC(parser); \
} while (0)

[[ nodiscard ]] ast_class* jack_parser_parse_class(jack_parser* parser);

[[ nodiscard ]] ast_var_dec* jack_parser_parse_class_var_dec(jack_parser* parser);
[[ nodiscard ]] ast_subroutine* jack_parser_parse_subroutine(jack_parser* parser);
[[ nodiscard ]] ast_var_dec* jack_parser_parse_param_list(jack_parser* parser);
[[ nodiscard ]] ast_var_dec* jack_parser_parse_var_dec(jack_parser* parser);
[[ nodiscard ]] ast_type jack_parser_parse_type(jack_parser* parser);

[[ nodiscard ]] ast_stmt* jack_parser_parse_statements(jack_parser* parser);
[[ nodiscard ]] ast_stmt* jack_parser_parse_let(jack_parser* parser);
[[ nodiscard ]] ast_stmt* jack_parser_parse_if(jack_parser* parser);
[[ nodiscard ]] ast_stmt* jack_parser_parse_while(jack_parser* parser);
[[ nodiscard ]] ast_stmt* jack_parser_parse_do(jack_parser* parser);
[[ nodiscard ]] ast_stmt* jack_parser_parse_return(jack_parser* parser);

[[ nodiscard ]] ast_expr* jack_parser_parse_expression(jack_parser* parser, binding_power min_bp);
[[ nodiscard ]] ast_expr* jack_parser_parse_term(jack_parser* parser);
[[ nodiscard ]] ast_expr* jack_parser_parse_subroutine_call(jack_parser* parser);
[[ nodiscard ]] ast_expr_list* jack_parser_parse_expression_list(jack_parser* parser);

[[ nodiscard ]] bool jack_parser_check(jack_parser* parser, int32_t type);
jack_token jack_parser_advance(jack_parser* parser);
jack_token jack_parser_expect(jack_parser* parser, int32_t type);
void jack_parser_sync(jack_parser* parser);

[[ nodiscard ]] ast_keyword_const jack_parser_token_type_to_keyword(jack_parser* parser, int32_t type);
[[ nodiscard ]] ast_unary_op jack_parser_token_type_to_unary_op(jack_parser* parser, int32_t type);
[[ nodiscard ]] ast_binary_op jack_parser_token_type_to_binary_op(jack_parser* parser, int32_t type);

#endif
