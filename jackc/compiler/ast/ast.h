#ifndef JACK_COMPILER_AST_H
#define JACK_COMPILER_AST_H

#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/allocators.h"
#include "jackc_string.h"
#include <stddef.h>
#include <stdint.h>

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
    jackc_string class_name;
} ast_type;

typedef enum {
    BINARY_OP_ADD, BINARY_OP_SUB,
    BINARY_OP_MUL, BINARY_OP_DIV,
    BINARY_OP_AND, BINARY_OP_OR,
    BINARY_OP_LT,  BINARY_OP_GT,
    BINARY_OP_EQ
} ast_binary_op;

typedef enum {
    UNARY_OP_NEG, // '-'
    UNARY_OP_NOT  // '~'
} ast_unary_op;

typedef enum {
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_NULL,
    KEYWORD_THIS
} ast_keyword_const;


typedef struct ast_expr ast_expr;
typedef struct ast_stmt ast_stmt;
typedef struct ast_expr_list ast_expr_list;

struct ast_expr_list {
    ast_expr* expr;
    ast_expr_list* next;
};

ast_expr_list* ast_expr_list_push_back(
    Allocator* allocator,
    ast_expr_list* tail,
    ast_expr* expr
);

ast_stmt* ast_stmt_list_push_back(
    ast_stmt* tail,
    ast_stmt* stmt
);

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
    bool implicit_this_receiver;
    jackc_string receiver;
    jackc_string subroutine_name;
    ast_expr_list* args;
} ast_call;

struct ast_expr {
    ast_base base;
    ast_expr_kind kind;

    union {
        int32_t int_val;
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

ast_expr* ast_expr_int(
    Allocator* allocator,
    jack_location* loc,
    int32_t value
);

ast_expr* ast_expr_string(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* value
);

ast_expr* ast_expr_keyword(
    Allocator* allocator,
    jack_location* loc,
    ast_keyword_const keyword
);

ast_expr* ast_expr_var(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* name
);

ast_expr* ast_expr_binary(
    Allocator* allocator,
    jack_location* loc,
    ast_expr* left,
    ast_binary_op op,
    ast_expr* right
);

ast_expr* ast_expr_unary(
    Allocator* allocator,
    jack_location* loc,
    ast_unary_op op,
    ast_expr* operand
);

ast_expr* ast_expr_call(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* receiver,
    const jackc_string* subroutine_name,
    ast_expr_list* args
);

ast_expr* ast_expr_array_access(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* var_name,
    ast_expr* index
);

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
    ast_stmt* next; // For lists of statements

    union {
        // let varName[index?] = value;
        struct {
            jackc_string var_name;
            ast_expr* index; // null if not an array access
            ast_expr* value;
        } let_stmt;

        // if (cond) { true_branch } else { false_branch? }
        struct {
            ast_expr* condition;
            ast_stmt* true_branch;
            ast_stmt* false_branch; // null if no 'else' block
        } if_stmt;

        // while (cond) { body }
        struct {
            ast_expr* cond;
            ast_stmt* body;
        } while_stmt;

        // do call;
        ast_call* do_stmt;

        // return value?;
        ast_expr* return_stmt; // null if returning void
    };
};

ast_stmt* ast_stmt_let(
    Allocator* a,
    jack_location* loc,
    const jackc_string* var_name,
    ast_expr* index,
    ast_expr* value
);

ast_stmt* ast_stmt_if(
    Allocator* a,
    jack_location* loc,
    ast_expr* cond,
    ast_stmt* true_branch,
    ast_stmt* false_branch
);

ast_stmt* ast_stmt_while(
    Allocator* a,
    jack_location* loc,
    ast_expr* cond,
    ast_stmt* body
);

ast_stmt* ast_stmt_do(
    Allocator* a,
    jack_location* loc,
    ast_call* subroutine_call
);

ast_stmt* ast_stmt_return(
    Allocator* a,
    jack_location* loc,
    ast_expr* value
);

typedef struct ast_var_dec ast_var_dec;

struct ast_var_dec {
    ast_base base;
    jack_variable_kind kind;
    ast_type type;
    jackc_string name;
    struct ast_var_dec* next; // Linked list of declarations
};

ast_var_dec* ast_var_dec_list_push_front(
    ast_var_dec* tail,
    ast_var_dec* stmt
);

ast_var_dec* ast_variable_declaration(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* name,
    jack_variable_kind kind,
    ast_type type,
    ast_var_dec* next
);


typedef enum {
    SUB_CONSTRUCTOR,
    SUB_FUNCTION,
    SUB_METHOD
} ast_sub_kind;

typedef struct ast_subroutine ast_subroutine;

struct ast_subroutine {
    ast_base base;
    ast_sub_kind kind;
    ast_type return_type;
    jackc_string name;

    ast_var_dec* params; // Linked list of VAR_ARG
    ast_var_dec* locals; // Linked list of VAR_LOCAL
    ast_stmt* body;      // Linked list of statements

    ast_subroutine* next; // Linked list of subroutines in class
};

ast_subroutine* ast_subroutine_create(
    Allocator* allocator,
    jack_location* loc,
    ast_sub_kind kind,
    const ast_type* return_type,
    const jackc_string* name,
    ast_var_dec* params,
    ast_var_dec* locals,
    ast_stmt* body,
    ast_subroutine* next
);

ast_subroutine* ast_subroutine_push_back(
    ast_subroutine* tail,
    ast_subroutine* subroutine
);

// The Root Node
typedef struct {
    ast_base base;
    jackc_string name;
    ast_var_dec* class_vars;      // Linked list of VAR_STATIC and VAR_FIELD
    ast_subroutine* subroutines;  // Linked list of methods/funcs/constructors
} ast_class;

ast_class* ast_class_create(
    Allocator* allocator,
    jack_location* loc,
    const jackc_string* name,
    ast_var_dec* class_vars,
    ast_subroutine* subroutines
);

#endif
