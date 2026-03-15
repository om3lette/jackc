#ifndef JACK_COMPILER_AST_H
#define JACK_COMPILER_AST_H

#include "compiler/lexer/compiler_lexer.h"
#include "jackc_string.h"
#include <stddef.h>

// ==================
// Helper types
// ==================

typedef enum {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_BOOLEAN,
    TYPE_VOID,
    TYPE_CLASS
} ast_type_kind;

typedef struct {
    ast_type_kind kind;
    jackc_string* class_name;
} ast_type;

typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
    OP_AND, OP_OR,  OP_LT,  OP_GT, OP_EQ
} ast_binary_op;

typedef enum {
    UNARY_OP_NEG, // '-'
    UNARY_OP_NOT  // '~'
} ast_unary_op;

typedef enum {
    KW_TRUE, KW_FALSE, KW_NULL, KW_THIS
} ast_keyword_const;


typedef struct ast_expr ast_expr;
typedef struct ast_stmt ast_stmt;

typedef struct ast_expr_list {
    ast_expr* expr;
    struct ast_expr_list* next;
} ast_expr_list;

typedef struct {
    jack_location loc;
} ast_base;

// ==================
// Expressions
// ==================

typedef enum {
    EXPR_INT,
    EXPR_STRING,
    EXPR_KEYWORD,
    EXPR_VAR,
    EXPR_ARRAY_ACCESS, // varName[expr]
    EXPR_CALL,         // subroutineCall
    EXPR_UNARY,
    EXPR_BINARY
} ast_expr_kind;

// Subroutine calls happen in expressions and 'do' statements
typedef struct {
    ast_base base;
    jackc_string* receiver; // Class name or Var name. null if implicit 'this'
    jackc_string subroutine_name;
    ast_expr_list* args;
} ast_call;

struct ast_expr {
    ast_base base;
    ast_expr_kind kind;

    union {
        int int_val;
        jackc_string string_val;
        ast_keyword_const keyword_val;
        jackc_string var_name;

        struct {
            jackc_string var_name;
            ast_expr* index;
        } array_access;

        ast_call call;

        struct {
            ast_unary_op op;
            ast_expr* operand;
        } unary;

        struct {
            ast_expr* left;
            ast_binary_op op;
            ast_expr* right;
        } binary;
    };
};

// ==================
// Statements
// ==================

typedef enum {
    STMT_LET,
    STMT_IF,
    STMT_WHILE,
    STMT_DO,
    STMT_RETURN
} ast_stmt_kind;

struct ast_stmt {
    ast_base base;
    ast_stmt_kind kind;
    struct ast_stmt* next; // For lists of statements

    union {
        // let varName[index?] = value;
        struct {
            jackc_string var_name;
            ast_expr* index; // null if not an array access
            ast_expr* value;
        } let_stmt;

        // if (cond) { true_branch } else { false_branch? }
        struct {
            ast_expr* cond;
            ast_stmt* true_branch;
            ast_stmt* false_branch; // null if no 'else' block
        } if_stmt;

        // while (cond) { body }
        struct {
            ast_expr* cond;
            ast_stmt* body;
        } while_stmt;

        // do call;
        ast_call do_stmt;

        // return value?;
        ast_expr* return_stmt; // null if returning void
    };
};

typedef struct ast_var_dec {
    ast_base base;
    jack_variable_kind kind;
    ast_type type;
    jackc_string name;
    struct ast_var_dec* next; // Linked list of declarations
} ast_var_dec;

typedef enum {
    SUB_CONSTRUCTOR,
    SUB_FUNCTION,
    SUB_METHOD
} ast_sub_kind;

typedef struct ast_subroutine {
    ast_base base;
    ast_sub_kind kind;
    ast_type return_type;
    jackc_string name;

    ast_var_dec* params; // Linked list of VAR_ARG
    ast_var_dec* locals; // Linked list of VAR_LOCAL
    ast_stmt* body;      // Linked list of statements

    struct ast_subroutine* next; // Linked list of subroutines in class
} ast_subroutine;

// The Root Node
typedef struct {
    ast_base base;
    jackc_string name;
    ast_var_dec* class_vars;      // Linked list of VAR_STATIC and VAR_FIELD
    ast_subroutine* subroutines;  // Linked list of methods/funcs/constructors
} ast_class;

#endif
