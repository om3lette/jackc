#include "compiler/ast/ast.h"
#include "compiler/ast/symtab_traversal.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "compiler/symtable/compiler_symtable.h"
#include "core/logging/logger.h"
#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include "jackc_string.h"
#include "frontend.h"

extern const jackc_diagnostic_translation diagnostic_translations[];

static jack_source* jack_source_add(
    jack_source* collection,
    ast_class* ast,
    const char* filepath,
    jackc_string content,
    uint32_t lines,
    Allocator* allocator
) {
    jack_source* node = allocator->alloc(sizeof(jack_source), allocator->context);
    node->ast = ast;
    node->filepath = filepath;
    node->content = content;
    node->lines = lines;
    node->next = collection;
    return node;
}

static jackc_parse_result jackc_parse_file(const char* filename, jackc_string source, Allocator* allocator) {
    jack_lexer lexer = jack_lexer_init(source.data);
    jackc_diagnostic_engine engine = jackc_diag_engine_init(source, filename, diagnostic_translations, 1);
    jack_parser parser = jack_parser_init(&lexer, &engine, allocator);

    ast_class* ast = jack_parser_parse_class(&parser);
    jackc_diagnostic_engine_report(&engine, lexer.line);

    return (jackc_parse_result){ .ast = ast, .lines = lexer.line, .had_error = parser.had_error };
}

static int build_global_symtable(
    const jack_source* source_files,
    sym_table* symbol_table,
    function_registry* func_registry
) {
    symtab_traversal_context symtab_ctx = {
        .symtab = symbol_table,
        .registry = func_registry,
        .engine = {},
        .had_redeclaration = false
    };
    const jack_source* current_file = source_files;
    while (current_file) {
        symtab_ctx.engine = jackc_diag_engine_init(current_file->content, current_file->filepath, diagnostic_translations, 1);
        ast_symtab_build_traversal(current_file->ast, &symtab_ctx);
        jackc_diagnostic_engine_report(&symtab_ctx.engine, current_file->lines);
        current_file = current_file->next;
    }
    if (symtab_ctx.had_redeclaration)
        return 1;

    return 0;
}

static void source_file_free(const jack_source* source_files) {
    while (source_files) {
        jackc_free((void*)source_files->content.data);
        jackc_free((void*)source_files->filepath);
        source_files = source_files->next;
    }
}

#define RETURN_WITH_CLEANUP(_code) do { \
    source_file_free(source_files); \
    return _code; \
} while (0);

jackc_frontend_return_code jackc_frontend_compile(const char* base_path, Allocator* allocator) {
    const char* source_file_path = nullptr;

    bool had_syntax_error = false;
    bool failed_to_open_source_file = false;
    jack_source* source_files = nullptr;

    while ((source_file_path = jackc_next_source_file(base_path, ".jack"))) {
        LOG_DEBUG("Parsing file %s...\n", source_file_path);
        const char* file_content_raw = jackc_read_file_content(source_file_path);

        if (!file_content_raw) {
            LOG_ERROR("Failed to open file at %s\n", source_file_path);
            failed_to_open_source_file = true;
            continue;
        }

        jackc_string file_content = jackc_string_from_str(file_content_raw);
        jackc_parse_result result = jackc_parse_file(source_file_path, file_content, allocator);

        had_syntax_error |= result.had_error;
        if (!result.ast) continue;

        source_files = jack_source_add(source_files, result.ast, source_file_path, file_content, result.lines, allocator);
    }
    // TODO: Proper return codes
    if (!source_files)
        RETURN_WITH_CLEANUP(FRONTEND_NO_SOURCE_FILES);
    if (had_syntax_error)
        RETURN_WITH_CLEANUP(FRONTEND_SYNTAX_ERROR);
    if (failed_to_open_source_file)
        RETURN_WITH_CLEANUP(FRONTEND_FAILED_TO_OPEN_SOURCE_FILE);

    // First ast traversal - build symbol table of class names and function signatures
    // Will report Class redeclarations
    sym_table* symbol_table = sym_table_init(nullptr, allocator);
    function_registry* registry = function_registry_init(allocator);
    if (build_global_symtable(source_files, symbol_table, registry))
        RETURN_WITH_CLEANUP(FRONTEND_SYMBOL_TABLE_BUILD_ERROR);

    RETURN_WITH_CLEANUP(FRONTEND_OK);
}
