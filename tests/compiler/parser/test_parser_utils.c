#include "test_parser_utils.h"
#include "test_lexer_common.h"
#include "compiler/ast/ast.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "core/localization/locale.h"
#include "core/asserts/jackc_assert.h"
#include "core/allocators/adapters.h"
#include "std/jackc_stdio.h"
#include "std/jackc_stdlib.h"
#include <stdarg.h>
#include <stdio.h>

char* jackc_itoa(int n, char* s, int base) {
    if (base < 2 || base > 36) {
        s[0] = '\0';
        return s;
    }

    char* p = s;

    // Handle sign (only for base 10)
    int negative = 0;
    if (base == 10 && n < 0) {
        negative = 1;
    }

    int num = (negative) ? -n : n;

    char tmp[33];
    char* q = tmp + sizeof(tmp) - 1;
    *q = '\0';

    do {
        char digit = (char)(num % base);
        *--q = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        num /= base;
    } while (num != 0);

    if (negative) {
        *p++ = '-';
    }
    while (*q) {
        *p++ = *q++;
    }
    *p = '\0';

    return s;
}

char* jackc_format(Allocator* allocator, const char* fmt, ...) {
    jackc_assert(allocator && fmt);

    va_list args;
    va_start(args, fmt);

    size_t length = 0;
    const char* p = fmt;
    va_list args_copy;

    va_copy(args_copy, args);
    while (*p) {
        if (*p == '%') {
            ++p;
            if (*p == 's') {
                const char* str = va_arg(args_copy, const char*);
                length += jackc_strlen(str);
            } else {
                ++length;
            }
            ++p;
        } else {
            ++length;
            ++p;
        }
    }
    va_end(args_copy);

    // +1 for '\0'
    char* buffer = allocator->alloc(length + 1, allocator->context);
    jackc_vsprintf(buffer, fmt, args);
    va_end(args);
    return buffer;
}

static const char* ast_binary_op_str(ast_binary_op op) {
    switch (op) {
        case BINARY_OP_ADD: return "+";
        case BINARY_OP_SUB: return "-";
        case BINARY_OP_MUL: return "*";
        case BINARY_OP_DIV: return "/";
        case BINARY_OP_AND: return "&";
        case BINARY_OP_OR:  return "|";
        case BINARY_OP_LT:  return "<";
        case BINARY_OP_GT:  return ">";
        case BINARY_OP_EQ:  return "=";
    }

    jackc_assert(false);
    return "";
}

static const char* ast_unary_op_str(ast_unary_op op) {
    switch (op) {
        case UNARY_OP_NEG: return "-";
        case UNARY_OP_NOT: return "~";
    }

    jackc_assert(false);
    return "";
}

static char* ast_keyword_str(ast_keyword_const keyword) {
    switch (keyword) {
        case KEYWORD_TRUE:  return "true";
        case KEYWORD_FALSE: return "false";
        case KEYWORD_NULL:  return "null";
        case KEYWORD_THIS:  return "this";
    }

    jackc_assert(false);
    return "";
}

static char* jack_string_to_c_str(jackc_string str, Allocator* allocator) {
    char* c_str = allocator->alloc(str.length + 1, allocator->context);
    jackc_memcpy(c_str, str.data, str.length);
    c_str[str.length] = '\0';
    return c_str;
}

const char* ast_expression_to_string(Allocator* allocator, const ast_expr* expr) {
    switch (expr->kind) {
        case EXPR_INT: {
            return jackc_itoa(
                expr->int_val,
                allocator->alloc(sizeof(char) * 33, allocator->context),
                10
            );
        }
        case EXPR_STRING: {
            return jack_string_to_c_str(expr->string_val, allocator);
        }
        case EXPR_KEYWORD: {
            return ast_keyword_str(expr->keyword_val);
        }
        case EXPR_VAR: {
            return jack_string_to_c_str(expr->var_name, allocator);
        }
        case EXPR_ARRAY_ACCESS: {
            return jackc_format(
                allocator,
                "%s[%s]",
                jack_string_to_c_str(expr->array_access.var_name, allocator),
                ast_expression_to_string(allocator, expr->array_access.index)
            );
        }
        case EXPR_CALL: {
            char* args_str = "";
            ast_expr_list* cur = expr->call.args;
            bool first = true;

            while (cur) {
                args_str = jackc_format(
                    allocator,
                    "%s%s%s",
                    args_str,
                    first ? "" : ", ",
                    ast_expression_to_string(allocator, cur->expr)
                );
                first = false;
                cur = cur->next;
            }
            char* subroutine_name_str = jack_string_to_c_str(expr->call.subroutine_name, allocator);

            if (!expr->call.implicit_this_receiver) {
                return jackc_format(
                    allocator,
                    "%s.%s(%s)",
                    jack_string_to_c_str(expr->call.receiver, allocator),
                    subroutine_name_str,
                    args_str
                );
            } else {
                return jackc_format(
                    allocator,
                    "this.%s(%s)",
                    subroutine_name_str,
                    args_str
                );
            }
        }
        case EXPR_UNARY: {
            return jackc_format(
                allocator,
                "%s(%s)",
                ast_unary_op_str(expr->unary.op),
                ast_expression_to_string(allocator, expr->unary.operand)
            );
        }
        case EXPR_BINARY: {
            return jackc_format(
                allocator,
                "(%s %s %s)",
                ast_expression_to_string(allocator, expr->binary.left),
                ast_binary_op_str(expr->binary.op),
                ast_expression_to_string(allocator, expr->binary.right)
            );
        }
    }

    jackc_assert(false);
    return "";
}

void test_parser_fixture_init(struct parser_fixture* tau, const char* source) {
    tau->arena = arena_allocator_adapter();
    tau->lexer = jack_lexer_init(source);
    tau->engine = jackc_diag_engine_init(tau->lexer.buffer, __FILE_NAME__, &jackc_locale_en, fileno(stdout));
}

void test_parser_fixture_destroy(struct parser_fixture* tau) {
    arena_allocator_destroy(tau->arena.context);
}

size_t var_len(ast_var_dec* var) {
    size_t len = 0;
    for (ast_var_dec* cur = var; cur != nullptr; cur = cur->next) {
        ++len;
    }
    return len;
}

size_t subroutine_len(ast_subroutine* sub) {
    size_t len = 0;
    for (ast_subroutine* cur = sub; cur != nullptr; cur = cur->next) {
        ++len;
    }
    return len;
}

size_t statements_len(ast_stmt* sub) {
    size_t len = 0;
    for (ast_stmt* cur = sub; cur != nullptr; cur = cur->next) {
        ++len;
    }
    return len;
}

static jack_parser* common_init(const char* src, struct parser_fixture* tau) {
    test_jack_lexer_new_buffer(&tau->lexer, src);
    jack_parser parser = jack_parser_init(&tau->lexer, &tau->engine, &tau->arena);
    tau->parser = parser;
    return &tau->parser;
}

ast_var_dec* parse_var(const char* src, struct parser_fixture* tau) {
    return jack_parser_parse_var_dec(common_init(src, tau));
}

ast_stmt* parse_statements(const char* src, struct parser_fixture* tau) {
    return jack_parser_parse_statements(common_init(src, tau));
}

ast_class* parse_class(const char* src, struct parser_fixture* tau) {
    return jack_parser_parse_class(common_init(src, tau));
}

ast_var_dec* parse_param_list(const char* src, struct parser_fixture* tau) {
    return jack_parser_parse_param_list(common_init(src, tau));
}

ast_subroutine* parse_subroutine(const char* src, struct parser_fixture* tau) {
    return jack_parser_parse_subroutine(common_init(src, tau));
}
