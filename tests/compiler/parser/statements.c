#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/allocators/adapters.h"
#include "core/allocators/allocators.h"
#include "jackc_stdlib.h"
#include "test_parser_utils.h"
#include "tau.h"


TEST_F_SETUP(parser_fixture) {
    tau->arena = arena_allocator_adapter();
    tau->lexer = jack_lexer_init("");
    tau->engine = jack_diag_engine_init();
    tau->parser = nullptr;
}

TEST_F_TEARDOWN(parser_fixture) {
    arena_allocator_destroy(tau->arena.context);
    jackc_free(tau->lexer);
}

TEST_F(parser_fixture, let_statement) {
    ast_stmt* stmt = parse_statements("let a = 5;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_LET);
    REQUIRE_JACK_STREQ(stmt->let_stmt.var_name, "a");
    REQUIRE_EQ(stmt->let_stmt.value->int_val, 5);
}

TEST_F(parser_fixture, let_statement_expression_assignment) {
    ast_stmt* stmt = parse_statements("let a = c + 10 * 3;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_LET);
    REQUIRE_EXPR(&tau->arena, stmt->let_stmt.value, "(c + (10 * 3))");
}

TEST_F(parser_fixture, let_statement_with_array_access) {
    ast_stmt* stmt = parse_statements("let test[Math.abs(-10 * 10)] = 5;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_LET);
    REQUIRE_JACK_STREQ(stmt->let_stmt.var_name, "test");
    REQUIRE_EQ(stmt->let_stmt.value->int_val, 5);
    REQUIRE_EXPR(&tau->arena, stmt->let_stmt.index, "Math.abs((-(10) * 10))");
}

TEST_F(parser_fixture, if_statement) {
    ast_stmt* stmt = parse_statements("if (2 < 3) { let a = 5; }", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_IF);
    REQUIRE_EXPR(&tau->arena, stmt->if_stmt.condition, "(2 < 3)");
    REQUIRE_STMT(stmt->if_stmt.true_branch, STMT_LET);
    REQUIRE_FALSE(stmt->if_stmt.true_branch->next);
    REQUIRE_FALSE(stmt->if_stmt.false_branch);
}

TEST_F(parser_fixture, if_statement_with_else) {
    ast_stmt* stmt = parse_statements("if (2 = 3) { let a = 5; } else { let b = 10; }", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_IF);
    REQUIRE_EXPR(&tau->arena, stmt->if_stmt.condition, "(2 = 3)");

    REQUIRE_STMT(stmt->if_stmt.true_branch, STMT_LET);
    REQUIRE_FALSE(stmt->if_stmt.true_branch->next);

    REQUIRE_STMT(stmt->if_stmt.false_branch, STMT_LET);
    REQUIRE_FALSE(stmt->if_stmt.false_branch->next);
}

TEST_F(parser_fixture, if_statement_with_else_and_multiple_stmts) {
    ast_stmt* stmt = parse_statements("if (2 = 3) { let a = 5; let b = 10; } else { let c = 15; let d = 20; }", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_IF);
    REQUIRE_EXPR(&tau->arena, stmt->if_stmt.condition, "(2 = 3)");

    REQUIRE_STMT(stmt->if_stmt.true_branch, STMT_LET);
    REQUIRE_STMT(stmt->if_stmt.true_branch->next, STMT_LET);

    REQUIRE_STMT(stmt->if_stmt.false_branch, STMT_LET);
    REQUIRE_STMT(stmt->if_stmt.false_branch->next, STMT_LET);
}


TEST_F(parser_fixture, while_statement) {
    ast_stmt* stmt = parse_statements("while (a < 10) { let a = a + 1; }", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_WHILE);
    REQUIRE_EXPR(&tau->arena, stmt->while_stmt.cond, "(a < 10)");
    REQUIRE_STMT(stmt->while_stmt.body, STMT_LET);
    REQUIRE_FALSE(stmt->while_stmt.body->next);
}

TEST_F(parser_fixture, while_statement_with_multiple_stmts) {
    ast_stmt* stmt = parse_statements("while (a < 10) { let a = a + 1; let b = a * 2; }", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_WHILE);
    REQUIRE_EXPR(&tau->arena, stmt->while_stmt.cond, "(a < 10)");
    REQUIRE_STMT(stmt->while_stmt.body, STMT_LET);
    REQUIRE_STMT(stmt->while_stmt.body->next, STMT_LET);
}

TEST_F(parser_fixture, do_statement) {
    ast_stmt* stmt = parse_statements("do Sys.wait(seconds * 1000);", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_DO);

    REQUIRE_JACK_STREQ(stmt->do_stmt->receiver, "Sys");
    REQUIRE_JACK_STREQ(stmt->do_stmt->subroutine_name, "wait");
    REQUIRE_EXPR(&tau->arena, stmt->do_stmt->args[0].expr, "(seconds * 1000)");
}

TEST_F(parser_fixture, return_statement_void) {
    ast_stmt* stmt = parse_statements("return;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_RETURN);

    REQUIRE_FALSE(stmt->return_stmt);
}

TEST_F(parser_fixture, return_statement_with_expr) {
    ast_stmt* stmt = parse_statements("return 2 + a * a;", tau);

    REQUIRE_NO_ERRORS(tau->parser);
    REQUIRE_STMT(stmt, STMT_RETURN);

    REQUIRE_TRUE(stmt->return_stmt);
    REQUIRE_EXPR(&tau->arena, stmt->return_stmt, "(2 + (a * a))");
}
