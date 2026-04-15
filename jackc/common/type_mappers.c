#include "type_mappers.h"
#include "compiler/ast/ast.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/asserts/jackc_assert.h"
#include "std/jackc_string.h"
#include "vm-translator/parser/vm_parser.h"

char* vm_segment_to_string(vm_segment seg) {
    switch (seg) {
        case SEGMENT_THIS: return "this";
        case SEGMENT_THAT: return "that";
        case SEGMENT_ARG: return "argument";
        case SEGMENT_LOCAL: return "local";
        case SEGMENT_CONSTANT: return "constant";
        case SEGMENT_STATIC: return "static";
        case SEGMENT_TEMP: return "temp";
        case SEGMENT_POINTER: return "pointer";
    }

    return "unknown";
}

char* token_type_to_str(jack_token_type token) {
    switch (token) {
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_CLASS:
            return "class";
        case TOKEN_CONSTRUCTOR:
            return "constructor";
        case TOKEN_FUNCTION:
            return "function";
        case TOKEN_METHOD:
            return "method";
        case TOKEN_FIELD:
            return "field";
        case TOKEN_STATIC:
            return "static";
        case TOKEN_VAR:
            return "var";
        case TOKEN_INT:
            return "int";
        case TOKEN_CHAR:
            return "char";
        case TOKEN_BOOLEAN:
            return "boolean";
        case TOKEN_VOID:
            return "void";
        case TOKEN_TRUE:
            return "true";
        case TOKEN_FALSE:
            return "false";
        case TOKEN_NULL:
            return "null";
        case TOKEN_THIS:
            return "this";
        case TOKEN_LET:
            return "let";
        case TOKEN_DO:
            return "do";
        case TOKEN_IF:
            return "if";
        case TOKEN_ELSE:
            return "else";
        case TOKEN_WHILE:
            return "while";
        case TOKEN_RETURN:
            return "return";
        case TOKEN_INT_LITERAL:
            return "<integer literal>";
        case TOKEN_STR_LITERAL:
            return "\"string literal\"";
        case TOKEN_IDENTIFIER:
            return "<identifier>";
        case TOKEN_NATIVE:
            return "native";
    }

    jackc_assert(false);
    return nullptr;
}

jackc_string ast_type_to_str(const ast_type* type) {
    switch (type->kind) {
        case TYPE_VOID: return jackc_string_from_str("void");
        case TYPE_INT: return jackc_string_from_str("int");
        case TYPE_BOOLEAN: return jackc_string_from_str("boolean");
        case TYPE_CHAR: return jackc_string_from_str("char");
        case TYPE_CLASS: return type->class_name;
    }

    jackc_assert(false);
    return jackc_string_empty();
}
