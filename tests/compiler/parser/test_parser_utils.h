#ifndef TEST_PARSER_UTILS_H
#define TEST_PARSER_UTILS_H

#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "core/allocators/allocators.h"

[[ nodiscard ]] const char* ast_expression_to_string(Allocator* allocator, const ast_expr* expr);

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
    REQUIRE(!parser->panic_mode); \
} while (0)

#define REQUIRE_PANIC(parser, errors_cnt) do { \
    REQUIRE(parser->panic_mode); \
    REQUIRE_ERRORS(parser, errors_cnt); \
} while (0)

#define REQUIRE_ERRORS(parser, count) do { \
    REQUIRE_EQ(parser->engine->size, count); \
} while (0)

#define REQUIRE_NO_ERRORS(parser) do { \
    REQUIRE_ERRORS(parser, 0); \
} while (0)

struct parser_fixture {
    Allocator arena;
    jack_lexer* lexer;
    jack_diagnostic_engine engine;
    jack_parser* parser;
};

#endif
