#include "compiler_parser_internal.h"
#include "compiler/ast/ast.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "compiler/parser/compiler_parser_diagnostics.h"
#include "core/asserts/jackc_assert.h"
#include "jackc_string.h"
#include <stdint.h>

static inline void enter_panic_mode(jack_parser* parser) {
    parser->had_error = true;
    parser->panic_mode = true;
}

static inline void exit_panic_mode(jack_parser* parser) {
    parser->panic_mode = false;
}

static inline bool is_panic_mode(jack_parser* parser) {
    return parser->panic_mode;
}

#define RETURN_IF_PANIC(parser) do { \
    if (is_panic_mode(parser)) \
        return nullptr; \
} while (0)

/**
 * Parses a list of variable declarations separated by commas.
 *
 * WILL NOT ENTER PANIC MODE if no declarations were found.
 * WILL NOT RETURN PARTIAL RESULTS ON ERROR
 */
static ast_var_dec* jack_parser_parse_variables(
    jack_parser* parser,
    int32_t end_token,
    jack_variable_kind kind,
    ast_type* type,
    bool updateType
) {
    jackc_assert(type && "type must be a valid pointer even if it will be discarded");

    ast_var_dec* declarations = nullptr;
    // It is possible to return partial results on an error
    // E.g: `var int x, y z` (missing colon between `y` and `z`. Semicolon is handled by the caller)
    // In this case x, y can be returned as partial results but current implementation prefers "atomicity" and will return nullptr
    while (
        !jack_parser_match(parser, TOKEN_EOF)
        && !jack_parser_match(parser, end_token)
    ) {
        // Expect a comma separator between variables (after the first one)
        if (declarations) {
            jack_parser_expect(parser, ',');
            if (is_panic_mode(parser))
                return nullptr;
        }

        if (updateType) {
            *type = jack_parser_parse_type(parser);
            if (is_panic_mode(parser))
                return nullptr;
        }

        // Expect a variable name
        jack_token var_name = jack_parser_expect(parser, TOKEN_IDENTIFIER);
        if (is_panic_mode(parser))
            return nullptr;

        declarations = ast_variable_declaration(
            parser->allocator, &var_name.loc, &var_name.str, kind, *type, declarations
        );
    }

    return declarations;
}

ast_var_dec* jack_parser_parse_class_var_dec(jack_parser* parser) {
    jack_variable_kind kind = VAR_STATIC;
    switch (parser->current.type) {
        case TOKEN_STATIC:
            kind = VAR_STATIC;
            break;
        case TOKEN_FIELD:
            kind = VAR_FIELD;
            break;
        default:
            enter_panic_mode(parser);
            jack_parser_diag_invalid_variable_type(parser);
            return nullptr;
    }
    jack_parser_advance(parser);

    ast_type type = jack_parser_parse_type(parser);
    if (is_panic_mode(parser))
        return nullptr;

    ast_var_dec* declarations = jack_parser_parse_variables(parser, ';', kind, &type, false);
    if (is_panic_mode(parser))
        return nullptr;
    // There were no declarations found, but the end token was reached with no unexpected tokens
    // Therefore variable name is missing, but the itself parser is in a valid state
    if (!is_panic_mode(parser) && !declarations) {
        parser->had_error = true;
        // TODO: Missing variable name
        jack_parser_diag_unexpected_token(parser, ';', TOKEN_IDENTIFIER);
    }

    jack_parser_expect(parser, ';');
    return declarations;
}

ast_subroutine* jack_parser_parse_subroutine(jack_parser* parser);

ast_var_dec* jack_parser_parse_param_list(jack_parser* parser) {
    if (jack_parser_match(parser, ')'))
        return nullptr;

    ast_type type;
    return jack_parser_parse_variables(parser, ')', VAR_ARG, &type, true);
}

ast_var_dec* jack_parser_parse_var_dec(jack_parser* parser) {
    jackc_assert(parser && "Parser is null");

    jack_parser_expect(parser, TOKEN_VAR);
    if (is_panic_mode(parser))
        return nullptr;

    ast_type type = jack_parser_parse_type(parser);
    if (is_panic_mode(parser))
        return nullptr;

    ast_var_dec* declarations = jack_parser_parse_variables(parser, ';', VAR_LOCAL, &type, false);
    if (is_panic_mode(parser))
        return nullptr;
    // There were no declarations found, but the end token was reached with no unexpected tokens
    // Therefore variable name is missing, but the itself parser is in a valid state
    if (!is_panic_mode(parser) && !declarations) {
        parser->had_error = true;
        // TODO: Missing variable name
        jack_parser_diag_unexpected_token(parser, ';', TOKEN_IDENTIFIER);
    }

    jack_parser_expect(parser, ';');
    return declarations;
}

ast_type jack_parser_parse_type(jack_parser* parser) {
    ast_type type;
    type.class_name = jackc_string_empty();
    jack_token type_token = jack_parser_advance(parser);
    switch (type_token.type) {
        case TOKEN_INT:
            type.kind = TYPE_INT;
            break;
        case TOKEN_CHAR:
            type.kind = TYPE_CHAR;
            break;
        case TOKEN_BOOLEAN:
            type.kind = TYPE_BOOLEAN;
            break;
        case TOKEN_IDENTIFIER:
            type.kind = TYPE_CLASS;
            type.class_name = type_token.str;
            break;
        default:
            enter_panic_mode(parser);
            jack_parser_diag_invalid_variable_type(parser);
    }
    return type;
}


ast_stmt* jack_parser_parse_statements(jack_parser* parser);
ast_stmt* jack_parser_parse_let(jack_parser* parser);
ast_stmt* jack_parser_parse_if(jack_parser* parser);
ast_stmt* jack_parser_parse_while(jack_parser* parser);
ast_stmt* jack_parser_parse_do(jack_parser* parser);
ast_stmt* jack_parser_parse_return(jack_parser* parser);

static inline bool get_infix_binding_power(int32_t type, binding_power* left_bp, binding_power* right_bp) {
    jackc_assert(left_bp && right_bp && "Binding power pointers must be valid");

    switch (type) {
        case '=':
            *left_bp = 1;
            *right_bp = 1;
            return true;
        case '<':
        case '>':
            *left_bp = 3;
            *right_bp = 4;
            return true;
        case '+':
        case '-':
            *left_bp = 5;
            *right_bp = 6;
            return true;
        case '*':
        case '/':
            *left_bp = 7;
            *right_bp = 8;
            return true;
        case '&':
        case '|':
            *left_bp = 2;
            *right_bp = 3;
            return true;
        default:
            return false;
    }
}

ast_expr* jack_parser_parse_expression(jack_parser* parser, binding_power min_bp) {
    jackc_assert(parser);

    ast_expr* lhs = jack_parser_parse_term(parser);
    RETURN_IF_PANIC(parser);

    while (true) {
        binding_power lbp, rbp;
        if (!get_infix_binding_power(parser->current.type, &lbp, &rbp))
            break;
        if (lbp < min_bp)
            break;

        jack_token op_token = jack_parser_advance(parser);
        ast_binary_op op = jack_parser_token_type_to_binary_op(parser, op_token.type);

        ast_expr* rhs = jack_parser_parse_expression(parser, rbp);
        RETURN_IF_PANIC(parser);

        lhs = ast_expr_binary(parser->allocator, &op_token.loc, lhs, op, rhs);
    }

    return lhs;
}

ast_expr* jack_parser_parse_term(jack_parser* parser) {
    jack_token cur_token = parser->current;
    ast_expr* expr = nullptr;

    switch (cur_token.type) {
        case TOKEN_INT_LITERAL: {
            expr = ast_expr_int(parser->allocator, &cur_token.loc, cur_token.value.integer);
            jack_parser_advance(parser);
            break;
        }
        case '"': {
            // '"'String literal'"'
            jack_parser_advance(parser);
            jack_token string_literal = jack_parser_expect(parser, TOKEN_STR_LITERAL);
            RETURN_IF_PANIC(parser);

            expr = ast_expr_string(parser->allocator, &string_literal.loc, &string_literal.str);
            break;
        }
        case TOKEN_TRUE:
        case TOKEN_FALSE:
        case TOKEN_NULL:
        case TOKEN_THIS: {
            ast_keyword_const keyword = jack_parser_token_type_to_keyword(parser, cur_token.type);
            RETURN_IF_PANIC(parser);

            expr = ast_expr_keyword(parser->allocator, &cur_token.loc, keyword);
            jack_parser_advance(parser);
            break;
        }
        case TOKEN_IDENTIFIER: {
            if (parser->next.type == '(' || parser->next.type == '.') {
                expr = jack_parser_parse_subroutine_call(parser);
            } else if (parser->next.type == '[') {
                // varName'['expression']'
                jack_token var_name = jack_parser_advance(parser);

                jack_parser_expect(parser, '[');
                RETURN_IF_PANIC(parser);

                ast_expr* index = jack_parser_parse_expression(parser, 0);
                RETURN_IF_PANIC(parser);

                jack_parser_expect(parser, ']');
                RETURN_IF_PANIC(parser);

                expr = ast_expr_array_access(parser->allocator, &cur_token.loc, &var_name.str, index);
            } else {
                // varName
                jack_token var_name = jack_parser_advance(parser);
                expr = ast_expr_var(parser->allocator, &cur_token.loc, &var_name.str);
            }
            break;
        }
        case '(': {
            jack_parser_advance(parser);

            expr = jack_parser_parse_expression(parser, 0);
            RETURN_IF_PANIC(parser);

            jack_parser_expect(parser, ')');
            break;
        }
        case '-':
        case '~': {
            jack_token op = jack_parser_advance(parser);

            ast_expr* operand = jack_parser_parse_expression(parser, PREFIX_BP);
            RETURN_IF_PANIC(parser);

            ast_unary_op op_type = jack_parser_token_type_to_unary_op(parser, op.type);
            RETURN_IF_PANIC(parser);

            expr = ast_expr_unary(parser->allocator, &op.loc, op_type, operand);
            break;
        }
        default:
            // TODO: Proper error
            enter_panic_mode(parser);
            jack_parser_diag_unexpected_token(parser, TOKEN_INT_LITERAL, cur_token.type);
            break;
    }
    return expr;
}

static inline ast_expr_list* parse_expression_list_with_parens(jack_parser* parser) {
    jack_parser_expect(parser, '(');
    RETURN_IF_PANIC(parser);

    ast_expr_list* args = jack_parser_parse_expression_list(parser);
    RETURN_IF_PANIC(parser);

    jack_parser_expect(parser, ')');
    RETURN_IF_PANIC(parser);

    return args;
}

ast_expr* jack_parser_parse_subroutine_call(jack_parser* parser) {
    jack_token first = jack_parser_expect(parser, TOKEN_IDENTIFIER);
    RETURN_IF_PANIC(parser);

    if (jack_parser_match(parser, '.')) {
        // first is a receiver
        jack_parser_advance(parser);

        jack_token subroutine = jack_parser_expect(parser, TOKEN_IDENTIFIER);
        RETURN_IF_PANIC(parser);

        ast_expr_list* args = parse_expression_list_with_parens(parser);
        RETURN_IF_PANIC(parser);

        return ast_expr_call(
            parser->allocator,
            &first.loc,
            &first.str,
            &subroutine.str,
            args
        );
    }

    // first is a subroutine name
    ast_expr_list* args = parse_expression_list_with_parens(parser);
    RETURN_IF_PANIC(parser);

    return ast_expr_call(
        parser->allocator,
        &first.loc,
        nullptr,
        &first.str,
        args
    );
}

ast_expr_list* jack_parser_parse_expression_list(jack_parser* parser) {
    ast_expr_list* head = nullptr;
    ast_expr_list* tail = nullptr;

    while (!jack_parser_match(parser, ')')) {
        ast_expr* expr = jack_parser_parse_term(parser);
        RETURN_IF_PANIC(parser);

        if (!head) {
            head = ast_expr_list_append(parser->allocator, head, expr);
            tail = head;
        } else {
            tail = ast_expr_list_append(parser->allocator, tail, expr);
        }
    }

    return head;
}

ast_subroutine* jack_parser_parse_subroutine(jack_parser* parser);

bool jack_parser_match(jack_parser* parser, int32_t type) {
    jackc_assert(parser && "Parser is null");

    return parser->current.type == type;
}

jack_token jack_parser_advance(jack_parser* parser) {
    jackc_assert(parser && "Parser is null");

    jack_token token = parser->current;

    parser->previous_token_type = token.type;
    parser->current = parser->next;
    parser->next = jack_lexer_next_token(parser->lexer);
    return token;
}

jack_token jack_parser_expect(jack_parser* parser, int32_t type) {
    jackc_assert(parser && "Parser is null");

    if (!jack_parser_match(parser, type)) {
        enter_panic_mode(parser);
        jack_parser_diag_unexpected_token(parser, type, parser->current.type);
        return parser->current;
    }
    return jack_parser_advance(parser);
}

jack_token jack_parser_expect_sync(jack_parser* parser, int32_t type) {
    jack_token token = jack_parser_expect(parser, type);
    if (is_panic_mode(parser))
        jack_parser_sync(parser);
    return token;
}

void jack_parser_sync(jack_parser* parser) {
    jackc_assert(parser && "Parser is null");

    if (parser->previous_token_type == ';')
        return;

    while (!jack_parser_match(parser, TOKEN_EOF)) {
        switch (parser->current.type) {
            case ';':
                exit_panic_mode(parser);
                jack_parser_advance(parser);
                return;
            case TOKEN_FUNCTION:
            case TOKEN_CONSTRUCTOR:
            case TOKEN_METHOD:
            case TOKEN_CLASS:
                exit_panic_mode(parser);
                return;
            default:
                jack_parser_advance(parser);
        }
    }
}

ast_keyword_const jack_parser_token_type_to_keyword(jack_parser* parser, int32_t type) {
    switch (type) {
        case TOKEN_TRUE:
            return KEYWORD_TRUE;
        case TOKEN_FALSE:
            return KEYWORD_FALSE;
        case TOKEN_NULL:
            return KEYWORD_NULL;
        case TOKEN_THIS:
            return KEYWORD_THIS;
        default:
            enter_panic_mode(parser);
            return KEYWORD_NULL;
    }
}

ast_unary_op jack_parser_token_type_to_unary_op(jack_parser* parser, int32_t type) {
    switch (type) {
        case '-':
            return UNARY_OP_NEG;
        case '~':
            return UNARY_OP_NOT;
        default:
            enter_panic_mode(parser);
            return UNARY_OP_NEG;
    }
}

ast_binary_op jack_parser_token_type_to_binary_op(jack_parser* parser, int32_t type) {
    switch (type) {
        case '+':
            return BINARY_OP_ADD;
        case '-':
            return BINARY_OP_SUB;
        case '*':
            return BINARY_OP_MUL;
        case '/':
            return BINARY_OP_DIV;
        case '&':
            return BINARY_OP_AND;
        case '|':
            return BINARY_OP_OR;
        case '<':
            return BINARY_OP_LT;
        case '>':
            return BINARY_OP_GT;
        case '=':
            return BINARY_OP_EQ;
        default:
            enter_panic_mode(parser);
            return BINARY_OP_ADD;
    }
}
