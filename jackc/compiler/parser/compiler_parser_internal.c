#include "compiler_parser_internal.h"
#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/diagnostic.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "compiler/parser/compiler_parser_utils.h"
#include "core/asserts/jackc_assert.h"
#include "std/jackc_string.h"
#include <stdint.h>

static ast_expr* jack_parser_parse_array_index(jack_parser* parser) {
    jack_parser_expect(parser, '[');
    RETURN_IF_PANIC(parser);

    ast_expr* index = jack_parser_parse_expression(parser, 0);
    RETURN_IF_PANIC(parser);

    jack_parser_expect(parser, ']');
    RETURN_IF_PANIC(parser);

    return index;
}

/**
 * Parses a list of variable declarations separated by commas.
 *
 * NEVER PANICS. Always attempts to recover and returns
 */
static ast_var_dec* jack_parser_parse_variables(
    jack_parser* parser,
    int32_t end_token,
    jack_variable_kind kind,
    ast_type* type,
    bool parse_type_on_every_iteration
) {
    jackc_assert(type && "type must be a valid pointer even if it will be discarded");
    jackc_assert((parser->sync_context & SYNC_VAR_DEC || parser->sync_context & SYNC_PARAM_LIST) && "Invalid sync context");

    bool is_first = true;
    ast_var_dec* declarations = nullptr;
    ast_var_dec* declarations_tail = nullptr;

    while (
        !is_rcurl_or_eof(parser)
        && !jack_parser_check(parser, end_token)
        && !is_class_member_start(parser)
    ) {
        // Expect a comma separator between variables (after the first one)
        if (!is_first) {
            jack_parser_expect(parser, ',');
            if (is_panic_mode(parser)) {
                jack_parser_sync(parser);
                continue;
            }
        }
        is_first = false;

        if (parse_type_on_every_iteration) {
            *type = jack_parser_parse_type(parser);
            if (is_panic_mode(parser)) {
                jack_parser_sync(parser);
                continue;
            }
        }

        // Expect a variable name
        jack_token var_name = jack_parser_expect(parser, TOKEN_IDENTIFIER);
        if (is_panic_mode(parser)) {
            jack_parser_sync(parser);
            continue;
        }

        ast_var_dec* new_node = ast_variable_declaration(
            parser->allocator, &var_name.str, kind, *type, nullptr
        );
        if (!declarations) {
            declarations = new_node;
            declarations_tail = new_node;
        } else {
            declarations_tail->next = new_node;
            declarations_tail = new_node;
        }
    }

    return declarations;
}

ast_class* jack_parser_parse_class(jack_parser* parser) {
    jack_parser_expect(parser, TOKEN_CLASS);
    RETURN_IF_PANIC(parser);

    jack_token class_name = jack_parser_expect(parser, TOKEN_IDENTIFIER);
    RETURN_IF_PANIC(parser);

    EXPECT_LEFT_CURLY_BRACE(parser);
    jack_sync_context_push(parser, SYNC_CLASS_BODY);

    ast_var_dec* class_vars = nullptr;
    ast_var_dec* class_vars_tail = nullptr;
    ast_subroutine* subroutines = nullptr;
    ast_subroutine* subroutines_tail = nullptr;
    bool seen_subroutine = false;

    while (!is_rcurl_or_eof(parser)) {
        if (!is_panic_mode(parser) && is_class_var_start(parser)) {
            if (seen_subroutine) {
                parser->had_error = true;
                jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_MIXING_DECLARATIONS_AND_CODE);
                jackc_diag_emit(&d);
            }
            class_vars_tail = ast_var_dec_list_push_back(
                class_vars_tail,
                jack_parser_parse_class_var_dec(parser)
            );
            if (!class_vars)
                class_vars = class_vars_tail;
            // Move the tail to the end of parsed list
            while (class_vars_tail && class_vars_tail->next)
                class_vars_tail = class_vars_tail->next;
        } else if (!is_panic_mode(parser) && is_subroutine_start(parser)) {
            seen_subroutine = true;
            subroutines_tail = ast_subroutine_push_back(
                subroutines_tail,
                jack_parser_parse_subroutine(parser)
            );
            if (!subroutines)
                subroutines = subroutines_tail;
        } else {
            jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_INVALID_TOKEN_CLASS_BODY);
            d.diag.data.invalid_token.got = parser->current.str;
            jackc_diag_emit(&d);

            jack_parser_sync(parser);
            // Synchronization stopped at the end of a subroutine
            // E.g `class C { var int badFunc() {} function void goodFunc() {} }`
            //                                   ^
            //                                   |
            //                               Sync token
            if (jack_parser_check(parser, '}') && parser->next.type != TOKEN_EOF) {
                jack_parser_advance(parser);
            }
        }
    }

    jack_sync_context_pop(parser, SYNC_CLASS_BODY);
    EXPECT_RIGHT_CURLY_BRACE(parser);

    return ast_class_create(
        parser->allocator,
        &class_name.str,
        class_vars,
        subroutines
    );
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

            jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_INVALID_VARIABLE_TYPE);
            jackc_diag_emit(&d);

            return nullptr;
    }
    jack_parser_advance(parser);

    ast_type type = jack_parser_parse_type(parser);
    RETURN_IF_PANIC(parser);

    jack_sync_context_push(parser, SYNC_VAR_DEC);
    ast_var_dec* declarations = jack_parser_parse_variables(parser, ';', kind, &type, false);
    jack_sync_context_pop(parser, SYNC_VAR_DEC);

    // There were no declarations found, but the end token was reached with no unexpected tokens
    // Therefore variable name is missing, but the itself parser is in a valid state
    if (!declarations) {
        parser->had_error = true;

        jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_MISSING_VARIABLE_NAME);
        jackc_diag_emit(&d);
    }

    EXPECT_SEMICOLON(parser);
    return declarations;
}

ast_subroutine* jack_parser_parse_subroutine(jack_parser* parser) {
    jack_sync_context_push(parser, SYNC_SUBROUTINE);

    bool is_native = false;
    if (jack_parser_check(parser, TOKEN_NATIVE)) {
        jack_parser_advance(parser);
        is_native = true;
    }

    ast_sub_kind sub_kind = SUB_FUNCTION;

    switch (parser->current.type) {
        case TOKEN_FUNCTION:
            sub_kind = SUB_FUNCTION;
            break;
        case TOKEN_CONSTRUCTOR:
            sub_kind = SUB_CONSTRUCTOR;
            break;
        case TOKEN_METHOD:
            sub_kind = SUB_METHOD;
            break;
        default:
            jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_INVALID_SUBROUTINE_KIND);
            jackc_diag_emit(&d);
            enter_panic_mode(parser);
            return nullptr;
    }
    jack_parser_advance(parser);

    ast_type type;
    switch (parser->current.type) {
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
            type.class_name = parser->current.str;
            break;
        case TOKEN_VOID:
            type.kind = TYPE_VOID;
            break;
        default:
            enter_panic_mode(parser);

            jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_INVALID_RETURN_TYPE);
            jackc_diag_emit(&d);

            return nullptr;
    }
    jack_parser_advance(parser);

    jack_token subroutine_name = jack_parser_expect(parser, TOKEN_IDENTIFIER);
    RETURN_IF_PANIC(parser);

    EXPECT_LEFT_PAREN(parser);
    ast_var_dec* params = jack_parser_parse_param_list(parser);
    EXPECT_RIGHT_PAREN(parser);


    ast_var_dec* locals = nullptr;
    ast_var_dec* locals_tail = nullptr;
    ast_stmt* body = nullptr;
    ast_stmt* stmt_tail = nullptr;
    bool seen_statement = false;
    EXPECT_LEFT_CURLY_BRACE(parser);

    while(
        !is_rcurl_or_eof(parser)
        && !is_class_member_start(parser)
    ) {
        if (!is_panic_mode(parser) && jack_parser_check(parser, TOKEN_VAR)) {
            if (seen_statement) {
                parser->had_error = true;
                jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_MIXING_DECLARATIONS_AND_CODE);
                jackc_diag_emit(&d);
            }
            locals_tail = ast_var_dec_list_push_back(
                locals_tail,
                jack_parser_parse_var_dec(parser)
            );
            if (!locals)
                locals = locals_tail;
            while (locals_tail && locals_tail->next)
                locals_tail = locals_tail->next;
        } else if (!is_panic_mode(parser) && is_statement_start(parser)) {
            seen_statement = true;
            stmt_tail = ast_stmt_list_push_back(
                stmt_tail,
                jack_parser_parse_statements(parser)
            );
            if (!body)
                body = stmt_tail;
        } else {
            jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_INVALID_TOKEN_SUBROUTINE_BODY);
            d.diag.data.invalid_token.got = parser->current.str;
            jackc_diag_emit(&d);
            jack_parser_sync(parser);
        }
    }
    jack_sync_context_pop(parser, SYNC_SUBROUTINE);
    EXPECT_RIGHT_CURLY_BRACE(parser);

    return ast_subroutine_create(
        parser->allocator,
        sub_kind,
        &type,
        &subroutine_name.str,
        params,
        locals,
        body,
        is_native,
        nullptr
    );
}

ast_var_dec* jack_parser_parse_param_list(jack_parser* parser) {
    jack_sync_context_push(parser, SYNC_PARAM_LIST);

    ast_type type;
    ast_var_dec* params = jack_parser_parse_variables(parser, ')', VAR_ARG, &type, true);

    jack_sync_context_pop(parser, SYNC_PARAM_LIST);
    return params;
}

ast_var_dec* jack_parser_parse_var_dec(jack_parser* parser) {
    jackc_assert(parser && "Parser is null");

    jack_parser_expect(parser, TOKEN_VAR);
    RETURN_IF_PANIC(parser);

    ast_type type = jack_parser_parse_type(parser);
    RETURN_IF_PANIC(parser);

    jack_sync_context_push(parser, SYNC_VAR_DEC);
    ast_var_dec* declarations = jack_parser_parse_variables(parser, ';', VAR_LOCAL, &type, false);
    jack_sync_context_pop(parser, SYNC_VAR_DEC);
    RETURN_IF_PANIC(parser);

    // There were no declarations found, but the end token was reached with no unexpected tokens
    // Therefore variable name is missing, but the itself parser is in a valid state
    if (!declarations) {
        parser->had_error = true;

        jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_MISSING_VARIABLE_NAME);
        jackc_diag_emit(&d);
    }

    EXPECT_SEMICOLON(parser);
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

            jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_INVALID_VARIABLE_TYPE);
            jackc_diag_emit(&d);
            return type;
    }
    return type;
}


ast_stmt* jack_parser_parse_statements(jack_parser* parser) {
    jack_sync_context_push(parser, SYNC_STATEMENT);

    ast_stmt* head = nullptr;
    ast_stmt* tail = nullptr;

    while (
        !jack_parser_check(parser, '}')
        && !jack_parser_check(parser, TOKEN_EOF)
    ) {
        ast_stmt* stmt = nullptr;

        switch (parser->current.type) {
            case TOKEN_LET:
                stmt = jack_parser_parse_let(parser);
                break;
            case TOKEN_DO:
                stmt = jack_parser_parse_do(parser);
                break;
            case TOKEN_IF:
                stmt = jack_parser_parse_if(parser);
                break;
            case TOKEN_WHILE:
                stmt = jack_parser_parse_while(parser);
                break;
            case TOKEN_RETURN:
                stmt = jack_parser_parse_return(parser);
                break;
            default:
                return head;
        }
        SYNC_IF_PANIC(parser);

        tail = ast_stmt_list_push_back(tail, stmt);
        if (!head) head = tail;
    }

    jack_sync_context_pop(parser, SYNC_STATEMENT);
    return head;
}

ast_stmt* jack_parser_parse_let(jack_parser* parser) {
    jack_parser_expect(parser, TOKEN_LET);
    RETURN_IF_PANIC(parser);

    jack_token var_token = jack_parser_expect(parser, TOKEN_IDENTIFIER);
    RETURN_IF_PANIC(parser);

    ast_expr* index = nullptr;
    if (jack_parser_check(parser, '[')) {
        index = jack_parser_parse_array_index(parser);
        RETURN_IF_PANIC(parser);
    }

    jack_parser_expect(parser, '=');
    RETURN_IF_PANIC(parser);

    ast_expr* value = jack_parser_parse_expression(parser, 0);
    RETURN_IF_PANIC(parser);

    EXPECT_SEMICOLON(parser);
    return ast_stmt_let(
        parser->allocator,
        &var_token.str,
        index,
        value
    );
}

ast_stmt* jack_parser_parse_if(jack_parser* parser) {
    jack_parser_expect(parser, TOKEN_IF);
    RETURN_IF_PANIC(parser);

    EXPECT_LEFT_PAREN(parser);
    ast_expr* condition = jack_parser_parse_expression(parser, 0);
    RETURN_IF_PANIC(parser);
    EXPECT_RIGHT_PAREN(parser);

    EXPECT_LEFT_CURLY_BRACE(parser);
    ast_stmt* true_branch = jack_parser_parse_statements(parser);
    RETURN_IF_PANIC(parser);
    EXPECT_RIGHT_CURLY_BRACE(parser);

    ast_stmt* false_branch = nullptr;
    if (jack_parser_check(parser, TOKEN_ELSE)) {
        jack_parser_advance(parser);
        EXPECT_LEFT_CURLY_BRACE(parser);
        false_branch = jack_parser_parse_statements(parser);
        RETURN_IF_PANIC(parser);
        EXPECT_RIGHT_CURLY_BRACE(parser);
    }

    return ast_stmt_if(
        parser->allocator,
        condition,
        true_branch,
        false_branch
    );
}

ast_stmt* jack_parser_parse_while(jack_parser* parser) {
    jack_parser_expect(parser, TOKEN_WHILE);
    RETURN_IF_PANIC(parser);

    EXPECT_LEFT_PAREN(parser);
    ast_expr* condition = jack_parser_parse_expression(parser, 0);
    RETURN_IF_PANIC(parser);
    EXPECT_RIGHT_PAREN(parser);

    EXPECT_LEFT_CURLY_BRACE(parser);
    ast_stmt* body = jack_parser_parse_statements(parser);
    RETURN_IF_PANIC(parser);
    EXPECT_RIGHT_CURLY_BRACE(parser);

    return ast_stmt_while(
        parser->allocator, condition, body
    );
}

ast_stmt* jack_parser_parse_do(jack_parser* parser) {
    jack_parser_expect(parser, TOKEN_DO);
    RETURN_IF_PANIC(parser);

    ast_expr* body = jack_parser_parse_subroutine_call(parser);
    RETURN_IF_PANIC(parser);

    EXPECT_SEMICOLON(parser);
    return ast_stmt_do(
        parser->allocator, &body->call
    );
}

ast_stmt* jack_parser_parse_return(jack_parser* parser) {
    jack_parser_expect(parser, TOKEN_RETURN);
    RETURN_IF_PANIC(parser);

    ast_expr* return_value = nullptr;

    if (!jack_parser_check(parser, ';')) {
        return_value = jack_parser_parse_expression(parser, 0);
        RETURN_IF_PANIC(parser);
    }

    EXPECT_SEMICOLON(parser);
    return ast_stmt_return(
        parser->allocator, return_value
    );
}

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

        lhs = ast_expr_binary(parser->allocator, lhs, op, rhs);
    }

    return lhs;
}

ast_expr* jack_parser_parse_term(jack_parser* parser) {
    jack_token cur_token = parser->current;
    ast_expr* expr = nullptr;

    switch (cur_token.type) {
        case TOKEN_INT_LITERAL: {
            expr = ast_expr_int(parser->allocator, cur_token.value.integer);
            jack_parser_advance(parser);
            break;
        }
        case TOKEN_STR_LITERAL: {
            // '"'String literal'"'
            jack_token string_literal = jack_parser_expect(parser, TOKEN_STR_LITERAL);
            RETURN_IF_PANIC(parser);

            expr = ast_expr_string(parser->allocator, &string_literal.str);
            break;
        }
        case TOKEN_TRUE:
        case TOKEN_FALSE:
        case TOKEN_NULL:
        case TOKEN_THIS: {
            ast_keyword_const keyword = jack_parser_token_type_to_keyword(parser, cur_token.type);
            RETURN_IF_PANIC(parser);

            expr = ast_expr_keyword(parser->allocator, keyword);
            jack_parser_advance(parser);
            break;
        }
        case TOKEN_IDENTIFIER: {
            if (parser->next.type == '(' || parser->next.type == '.') {
                expr = jack_parser_parse_subroutine_call(parser);
            } else if (parser->next.type == '[') {
                // varName'['expression']'
                jack_token var_name = jack_parser_advance(parser);

                ast_expr* index = jack_parser_parse_array_index(parser);
                RETURN_IF_PANIC(parser);

                expr = ast_expr_array_access(parser->allocator, &var_name.str, index);
            } else {
                // varName
                jack_token var_name = jack_parser_advance(parser);
                expr = ast_expr_var(parser->allocator, &var_name.str);
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

            expr = ast_expr_unary(parser->allocator, op_type, operand);
            break;
        }
        default:
            enter_panic_mode(parser);
            jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_INVALID_TOKEN_TERM);
            d.diag.data.invalid_token.got = parser->current.str;
            jackc_diag_emit(&d);
            break;
    }
    return expr;
}

static inline ast_expr_list* parse_expression_list_with_parens(jack_parser* parser) {
    EXPECT_LEFT_PAREN(parser);

    ast_expr_list* args = jack_parser_parse_expression_list(parser);
    RETURN_IF_PANIC(parser);

    EXPECT_RIGHT_PAREN(parser);
    return args;
}

ast_expr* jack_parser_parse_subroutine_call(jack_parser* parser) {
    jack_token first = jack_parser_expect(parser, TOKEN_IDENTIFIER);
    RETURN_IF_PANIC(parser);

    if (jack_parser_check(parser, '.')) {
        // first is a receiver
        jack_parser_advance(parser);

        jack_token subroutine = jack_parser_expect(parser, TOKEN_IDENTIFIER);
        RETURN_IF_PANIC(parser);

        ast_expr_list* args = parse_expression_list_with_parens(parser);
        RETURN_IF_PANIC(parser);

        return ast_expr_call(
            parser->allocator,
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
        nullptr,
        &first.str,
        args
    );
}

ast_expr_list* jack_parser_parse_expression_list(jack_parser* parser) {
    ast_expr_list* head = nullptr;
    ast_expr_list* tail = nullptr;

    while (!jack_parser_check(parser, ')')) {
        if (head) {
            jack_parser_expect(parser, ',');
            RETURN_IF_PANIC(parser);
        }

        ast_expr* expr = jack_parser_parse_expression(parser, 0);
        RETURN_IF_PANIC(parser);

        tail = ast_expr_list_push_back(parser->allocator, tail, expr);
        if (!head) head = tail;
    }

    return head;
}

bool jack_parser_check(jack_parser* parser, int32_t type) {
    jackc_assert(parser && "Parser is null");

    return parser->current.type == type;
}

jack_token jack_parser_advance(jack_parser* parser) {
    jackc_assert(parser && "Parser is null");

    jack_token token = parser->current;

    parser->previous = token;
    parser->current = parser->next;
    parser->next = jack_lexer_next_token(parser->lexer);
    return token;
}

jack_token jack_parser_expect(jack_parser* parser, int32_t type) {
    jackc_assert(parser && "Parser is null");

    if (!jack_parser_check(parser, type)) {
        enter_panic_mode(parser);

        jackc_diag_builder d = diagnostic_begin(parser, DIAG_ERROR, DIAG_UNEXPECTED_TOKEN);
        switch (type) {
            case ';':
                // For ';' we need to now the location of the previous token to display where the ';' was expected
                d.diag.span = parser->previous.span;
                d.diag.code = DIAG_MISSING_SEMICOLON;
                d.diag.data.last_valid_token = (typeof(d.diag.data.last_valid_token)) {
                    .token = parser->previous.str
                };
                break;
            default:
                d.diag.data.unexpected_token = (typeof(d.diag.data.unexpected_token)) {
                    .expected = type, .got = parser->current.str
                };
                break;
        }
        jackc_diag_emit(&d);

        return parser->current;
    }
    return jack_parser_advance(parser);
}

static bool is_sync_token(jack_parser* parser) {
    jackc_assert(parser && "Parser is null");

    int32_t type = parser->current.type;
    if (type == TOKEN_EOF) return true;
    jack_sync_context context = parser->sync_context;

    if (context & SYNC_PARAM_LIST) {
        if (type == ',' || type == ')') return true;
    }
    if (context & SYNC_VAR_DEC) {
        if (type == ',' || type == ';') return true;
    }
    if (context & SYNC_STATEMENT) {
        switch (type) {
            case TOKEN_LET:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
            case TOKEN_DO:
            case '}':
                return true;
            default:
                break;
        }
    }
    if (context & SYNC_SUBROUTINE) {
        switch (type) {
            case TOKEN_METHOD:
            case TOKEN_FUNCTION:
            case TOKEN_CONSTRUCTOR:
            case TOKEN_NATIVE:
            case '}':
                return true;
            default:
                break;
        }
    }
    if (context & SYNC_CLASS_BODY) {
        switch (type) {
            case TOKEN_FIELD:
            case TOKEN_STATIC:
            case TOKEN_METHOD:
            case TOKEN_FUNCTION:
            case TOKEN_CONSTRUCTOR:
            case TOKEN_NATIVE:
            case '}':
                return true;
            default:
                break;
        }
    }

    return false;
}

void jack_parser_sync(jack_parser* parser) {
    jackc_assert(parser && "Parser is null");

    while (!is_sync_token(parser)) {
        jack_parser_advance(parser);
    }

    parser->had_error = true;
    exit_panic_mode(parser);
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
