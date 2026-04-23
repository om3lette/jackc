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
void jackc_diag_engine_reset(
    jackc_diagnostic_engine* engine,
    jackc_string source,
    const char* filename,
    FD output_fd
);

void jackc_diagnostic_engine_report(jackc_diagnostic_engine* engine, uint32_t lines_total);

typedef struct {
    jackc_diagnostic_engine* engine;
    jackc_diagnostic diag;
} jackc_diag_builder;

[[ nodiscard ]] jackc_diag_builder jackc_diag_begin(
    jackc_diagnostic_engine* engine,
    jackc_diagnostic_severity severity,
    jackc_diagnostic_code code,
    jackc_span span
);
[[ nodiscard ]] jackc_diag_builder jackc_diag_begin_str(
    jackc_diagnostic_engine* engine,
    jackc_diagnostic_severity severity,
    jackc_diagnostic_code code,
    const jackc_string* str
);

void jackc_diag_add_note(
    jackc_diag_builder* builder,
    jackc_diagnostic_code code,
    jackc_span span,
    Allocator* allocator
);
void jackc_diag_add_note_str(
    jackc_diag_builder* builder,
    jackc_diagnostic_code code,
    const jackc_string* str,
    Allocator* allocator
);

void jackc_diag_emit(const jackc_diag_builder* builder);
void jackc_diag_push(jackc_diagnostic_engine* engine, jackc_diagnostic diagnostic);


#endif
