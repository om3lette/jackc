#include "compiler/ast/ast.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "core/logging/logger.h"
#include "jackc_stdio.h"
#include "jackc_stdlib.h"
#include "jackc_string.h"
#include "frontend.h"

extern const jackc_diagnostic_translation diagnostic_translations[];

static jack_ast_collection* jack_ast_collection_add(
    jack_ast_collection* collection,
    ast_class* ast,
    jackc_string source,
    Allocator* allocator
) {
    jack_ast_collection* node = allocator->alloc(sizeof(jack_ast_collection), allocator->context);
    node->ast = ast;
    node->source = source;
    node->next = collection;
    return node;
}

jackc_parse_result jackc_parse_file(const char* filename, jackc_string source, Allocator* allocator) {
    jack_lexer lexer = jack_lexer_init(source.data);
    jackc_diagnostic_engine engine = jackc_diag_engine_init(source, filename, diagnostic_translations, 1);
    jack_parser parser = jack_parser_init(&lexer, &engine, allocator);

    ast_class* ast = jack_parser_parse_class(&parser);
    jackc_diagnostic_engine_report(&engine, lexer.line);

    return (jackc_parse_result){ .ast = ast, .had_error = parser.had_error };
}

int jackc_frontend_compile(const char* base_path, Allocator* allocator) {
    const char* source_file_path = nullptr;

    bool had_error = false;
    jack_ast_collection* asts = nullptr;

    while ((source_file_path = jackc_next_source_file(base_path, ".jack"))) {
        LOG_DEBUG("Parsing file %s...\n", source_file_path);
        const char* file_content_raw = jackc_read_file_content(source_file_path);
        jackc_free((void*)source_file_path);
        if (!file_content_raw) {
            LOG_ERROR("Failed to open file at %s\n", source_file_path);
            had_error = true;
            continue;
        }

        jackc_string file_content = jackc_string_from_str(file_content_raw);
        jackc_parse_result result = jackc_parse_file(source_file_path, file_content, allocator);

        had_error |= result.had_error;
        if (!result.ast) continue;

        asts = jack_ast_collection_add(asts, result.ast, file_content, allocator);
    }
    if (had_error)
        return 1;


    while (asts) {
        jackc_free((void*)asts->source.data);
        asts = asts->next;
    }
    return 0;
}
