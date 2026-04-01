#ifndef TEST_PARSER_UTILS_H
#define TEST_PARSER_UTILS_H

#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "core/allocators/allocators.h"

[[ nodiscard ]] const char* ast_expression_to_string(Allocator* allocator, const ast_expr* expr);

#define REQUIRE_JACK_STREQ(_jack_str, _expected_c_str) do { \
    REQUIRE(jackc_streq(&_jack_str, _expected_c_str)); \
} while (0)

#define REQUIRE_STMT(_stmt, _kind) do { \
    REQUIRE(_stmt); \
    REQUIRE_EQ(_stmt->kind, _kind); \
} while (0)

#define REQUIRE_EXPR(_a, _expr, _expected_str) do { \
    REQUIRE(_expr); \
    REQUIRE_STREQ(ast_expression_to_string(_a, _expr), _expected_str); \
} while (0)

#define REQUIRE_VAR(_var, _kind, _type, _name) do { \
    REQUIRE(_var); \
    REQUIRE_EQ(_var->kind, _kind); \
    REQUIRE_EQ(_var->type.kind, _type); \
    REQUIRE(jackc_streq(&_var->name, _name)); \
} while (0)

#define REQUIRE_CLASS_VAR(_var, _kind, _classname, _name) do { \
    REQUIRE(_var); \
    REQUIRE_EQ(_var->kind, _kind); \
    REQUIRE_EQ(_var->type.kind, TYPE_CLASS); \
    REQUIRE(jackc_streq(&_var->type.class_name, _classname)); \
    REQUIRE(jackc_streq(&_var->name, _name)); \
} while (0)

#define REQUIRE_NO_PANIC(parser) do { \
    REQUIRE(!parser.panic_mode); \
} while (0)

#define REQUIRE_PANIC(parser, errors_cnt) do { \
    REQUIRE(parser.panic_mode); \
    REQUIRE_ERRORS(parser, errors_cnt); \
} while (0)

#define REQUIRE_ERRORS(parser, count) do { \
    REQUIRE_EQ(parser.engine->size, count); \
} while (0)

#define REQUIRE_NO_ERRORS(parser) do { \
    REQUIRE_ERRORS(parser, 0); \
} while (0)

struct parser_fixture {
    Allocator arena;
    jack_lexer lexer;
    jackc_diagnostic_engine engine;
    jack_parser parser;
};

void test_parser_fixture_init(struct parser_fixture* tau, const char* source);
void test_parser_fixture_destroy(struct parser_fixture* tau);

[[ nodiscard ]] size_t var_len(ast_var_dec* var);
[[ nodiscard ]] size_t subroutine_len(ast_subroutine* sub);
[[ nodiscard ]] size_t statements_len(ast_stmt* sub);

ast_var_dec* parse_var(const char* src, struct parser_fixture* tau);
ast_stmt* parse_statements(const char* src, struct parser_fixture* tau);
ast_class* parse_class(const char* src, struct parser_fixture* tau);
ast_var_dec* parse_param_list(const char* src, struct parser_fixture* tau);
ast_subroutine* parse_subroutine(const char* src, struct parser_fixture* tau);

#endif
