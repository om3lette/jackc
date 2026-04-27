#ifndef JACKC_COMPILER_DIAGNOSTICS_ENGINE_ENGINE_H
#define JACKC_COMPILER_DIAGNOSTICS_ENGINE_ENGINE_H

#include "core/localization/locale.h"
#include "core/allocators/allocators.h"
#include "std/jackc_string.h"
#include "std/jackc_syscalls.h"
#include "diagnostic.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t line, col;
} jack_location;

typedef struct {
    jackc_string source;
    const char* filename;
    const jackc_locale* locale;
    FD output_fd;

    size_t size;
    bool overflow;
    jackc_diagnostic diagnostics[MAX_DIAGNOSTICS];
} jackc_diagnostic_engine;

[[ nodiscard ]] jackc_diagnostic_engine jackc_diag_engine_init(
    jackc_string source,
    const char* filename,
    const jackc_locale* locale,
    FD output_fd,
    bool override_filename
);

/**
 * Resets the diagnostic engine with a new source and filename.
 * Also resets the diagnostic idx effectively clearing any existing diagnostics.
 * 
 * @param engine The diagnostic engine to reset.
 * @param source The new source string.
 * @param filename The new filename.
 * @param output_fd The output file descriptor.
 */
void jackc_diag_engine_reset(
    jackc_diagnostic_engine* engine,
    jackc_string source,
    const char* filename,
    FD output_fd
);

/**
 * Reports all diagnostics in the engine to the output file descriptor.
 * 
 * @param engine The diagnostic engine to report from.
 * @param lines_total The total number of lines in the source.
 */
void jackc_diagnostic_engine_report(jackc_diagnostic_engine* engine, uint32_t lines_total);

typedef struct {
    jackc_diagnostic_engine* engine;
    jackc_diagnostic diag;
} jackc_diag_builder;

/**
 * Begins a diagnostic builder with the given severity, code, and span.
 * 
 * @param engine The diagnostic engine to use.
 * @param severity The severity of the diagnostic.
 * @param code The diagnostic code.
 * @param span The span of the diagnostic.
 * @return A diagnostic builder.
 */
[[ nodiscard ]] jackc_diag_builder jackc_diag_begin(
    jackc_diagnostic_engine* engine,
    jackc_diagnostic_severity severity,
    jackc_diagnostic_code code,
    jackc_span span
);

/**
 * Begins a diagnostic builder with the given severity, code, and string.
 * Constructs a span from the given string under the hood.
 * 
 * @param engine The diagnostic engine to use.
 * @param severity The severity of the diagnostic.
 * @param code The diagnostic code.
 * @param str The string of the diagnostic.
 * @return A diagnostic builder.
 */
[[ nodiscard ]] jackc_diag_builder jackc_diag_begin_str(
    jackc_diagnostic_engine* engine,
    jackc_diagnostic_severity severity,
    jackc_diagnostic_code code,
    const jackc_string* str
);

/**
 * Adds a note to the diagnostic builder.
 * 
 * @param builder The diagnostic builder to add the note to.
 * @param code The diagnostic code of the note.
 * @param span The span of the note.
 * @param allocator The allocator to use for the note.
 */
void jackc_diag_add_note(
    jackc_diag_builder* builder,
    jackc_diagnostic_code code,
    jackc_span span,
    Allocator* allocator
);

/**
 * Adds a note to the diagnostic builder with the given code and string.
 * Constructs a span from the given string under the hood.
 * 
 * @param builder The diagnostic builder to add the note to.
 * @param code The diagnostic code of the note.
 * @param str The string of the note.
 * @param allocator The allocator to use for the note.
 */
void jackc_diag_add_note_str(
    jackc_diag_builder* builder,
    jackc_diagnostic_code code,
    const jackc_string* str,
    Allocator* allocator
);

/**
 * Stores the diagnostic built by the given builder.
 * 
 * @param builder The diagnostic builder to store.
 */
void jackc_diag_emit(const jackc_diag_builder* builder);

/**
 * Pushes the diagnostic built by the given builder onto the diagnostic engine.
 * If the diagnostics engine is full, the diagnostic is dropped and `overflow` flag is set.
 * 
 * @param engine The diagnostic engine to push the diagnostic onto.
 * @param diagnostic The diagnostic to push.
 */
void jackc_diag_push(jackc_diagnostic_engine* engine, jackc_diagnostic diagnostic);


#endif
