#include "compiler/ast/ast.h"
#include "compiler/ast/traversals.h"
#include "compiler/diagnostics-engine/diagnostic.h"
#include "compiler/diagnostics-engine/engine.h"
#include "compiler/function-registry/function_registry.h"
#include "compiler/lexer/compiler_lexer.h"
#include "compiler/parser/compiler_parser.h"
#include "compiler/parser/compiler_parser_internal.h"
#include "compiler/symtable/compiler_symtable.h"
#include "core/allocators/allocators.h"
#include "core/asserts/jackc_assert.h"
#include "core/logging/logger.h"
#include "std/jackc_stdio.h"
#include "std/jackc_stdlib.h"
#include "std/jackc_string.h"
#include "std/jackc_syscalls.h"
#include "core/jackc_file_utils.h"
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

static bool build_global_symtable(
    const jack_source* source_files,
    function_registry* func_registry
) {
    function_registry_traversal_context symtab_ctx = {
        .registry = func_registry,
        .engine = {},
        .had_redeclaration = false
    };
    const jack_source* current_file = source_files;
    while (current_file) {
        symtab_ctx.engine = jackc_diag_engine_init(current_file->content, current_file->filepath, diagnostic_translations, 1);
        ast_function_registry_build_traversal(current_file->ast, &symtab_ctx);
        jackc_diagnostic_engine_report(&symtab_ctx.engine, current_file->lines);
        current_file = current_file->next;
    }
    return symtab_ctx.had_redeclaration;
}

static bool is_semantically_invalid(
    const jack_source* source_files,
    function_registry* registry,
    Allocator* allocator
) {
    bool is_invalid = false;
    for (const jack_source* current_file = source_files; current_file; current_file = current_file->next) {
        jackc_diagnostic_engine engine = jackc_diag_engine_init(current_file->content, current_file->filepath, diagnostic_translations, 1);
        semantic_validity_traversal_context ctx = semantic_validity_traversal_context_init(
            &current_file->ast->name, registry, &engine, allocator
        );
        is_invalid |= ast_semantic_validity_traversal(current_file->ast, &ctx);

        jackc_string filename = jackc_find_filename_no_ext(current_file->filepath);
        if (!filename.data) {
            LOG_ERROR("Failed to extract filename from %s\n", current_file->filepath);
            is_invalid = true;
            continue;
        }
        if (jackc_string_cmp(&filename, &current_file->ast->name) != 0) {
            jackc_diag_builder d = jackc_diag_begin(
                &ctx.engine, DIAG_ERROR, DIAG_CLASS_NAME_DOES_NOT_MATCH_THE_FILENAME, current_file->ast->name
            );
            d.diag.data.expected_class_name.filename = filename;
            jackc_diag_emit(&d);
            is_invalid = true;
        }

        jackc_diagnostic_engine_report(&ctx.engine, current_file->lines);
    }

    return is_invalid;
}

static bool generate_vm_code(
    const char* out_base_dir,
    const jack_source* source_files,
    const function_registry* registry,
    Allocator* allocator
) {
    // There is no reliable way to create directories while running in RARS
    // The solution is to flatten the directory structure for .vm files
    char out_file_path[4096];
    bool had_error = false;

    // Label indexes need to persist through the whole compilation
    vm_code_generation_traversal_context ctx = (vm_code_generation_traversal_context){
        .fd = -1,
        .registry = registry,
        .symtab = sym_table_init(nullptr, allocator),
        .allocator = allocator,
        .this = ast_variable_declaration(
            allocator, &jackc_string_from_str("this"), VAR_ARG, (ast_type){}, nullptr
        ),
        .if_label_index = 0,
        .while_label_index = 0
    };
    for (const jack_source* current_file = source_files; current_file; current_file = current_file->next) {
        jackc_string filename = jackc_find_filename_no_ext(current_file->filepath);
        if (!filename.data) {
            LOG_ERROR("Failed to extract filename from %s\n", current_file->filepath);
            had_error = true;
            continue;
        }
        jackc_sprintf(
            out_file_path,
            "%s/%.*s.vm",
            out_base_dir,
            filename.length, filename.data
        );
        int fd = jackc_open(out_file_path, O_CREAT | O_WRONLY | O_TRUNC);
        if (fd < 0) {
            had_error = true;
            continue;
        }
        ctx.fd = fd;
        ctx.had_error = false;

        jackc_assert(ctx.fd != -1 && !ctx.had_error && "Context state is invalid");
        vm_code_genetation_traversal(current_file->ast, &ctx);
        had_error |= ctx.had_error;

        jackc_close(fd);
    }

    return had_error;
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

jackc_frontend_return_code jackc_frontend_compile(
    const char* input_paths[],
    uint32_t n_paths,
    const char* output_dir,
    Allocator* allocator,
    bool skip_vm_code_gen
) {
    const char* source_file_path = nullptr;

    bool had_syntax_error = false;
    bool failed_to_open_source_file = false;
    jack_source* source_files = nullptr;

    for (size_t i = 0; i < n_paths; ++i) {
        const char* current_path = input_paths[i];
        if (!current_path)
            continue;

        while ((source_file_path = jackc_next_source_file(current_path, ".jack"))) {
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

            source_files = jack_source_add(
                source_files,
                result.ast,
                source_file_path,
                file_content,
                result.lines,
                allocator
            );
        }
    }
    // Report syntax error before source files
    // Because if all files had an error there will be no source files, which will cause
    // FRONTEND_NO_SOURCE_FILES to be mistakenly returned
    if (had_syntax_error)
        RETURN_WITH_CLEANUP(FRONTEND_SYNTAX_ERROR);
    if (!source_files)
        RETURN_WITH_CLEANUP(FRONTEND_NO_SOURCE_FILES);
    if (failed_to_open_source_file)
        RETURN_WITH_CLEANUP(FRONTEND_FAILED_TO_OPEN_SOURCE_FILE);

    // The first AST pass - build symbol table of class names and function signatures
    // Will report Class redeclarations
    function_registry* registry = function_registry_init(allocator);
    if (build_global_symtable(source_files, registry))
        RETURN_WITH_CLEANUP(FRONTEND_SYMBOL_TABLE_BUILD_ERROR);

    // The second AST pass - check that program is semantically valid
    if (is_semantically_invalid(source_files, registry, allocator))
        RETURN_WITH_CLEANUP(FRONTEND_SEMANTICALLY_INVALID);

    if (!skip_vm_code_gen) {
        // The third AST pass - knowing that the code is valid, generate the vm code
        if (generate_vm_code(output_dir, source_files, registry, allocator))
            RETURN_WITH_CLEANUP(FRONTEND_FAILED_TO_GENERATE_VM_CODE);
    }

    RETURN_WITH_CLEANUP(FRONTEND_OK);
}
