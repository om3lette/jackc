#ifndef JACKC_COMPILER_DIAGNOSTICS_ENGINE_ENGINE_H
#define JACKC_COMPILER_DIAGNOSTICS_ENGINE_ENGINE_H

#include "compiler/diagnostics-engine/translations/translation.h"
#include "jackc_string.h"
#include "diagnostic.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    jackc_string source;
    const char* filename;
    const jackc_diagnostic_translation* translations;

    size_t size;
    bool overflow;
    jackc_diagnostic diagnostics[MAX_DIAGNOSTICS];
} jackc_diagnostic_engine;

jackc_diagnostic_engine jack_diag_engine_init(jackc_string source, const char* filename, const jackc_diagnostic_translation* translations);
void jackc_diagnostic_engine_report(jackc_diagnostic_engine* engine, uint32_t lines_total);

typedef struct {
    jackc_diagnostic_engine* engine;
    jackc_diagnostic diag;
} jackc_diag_builder;

jackc_diag_builder jackc_diag_begin(
    jackc_diagnostic_engine* engine,
    jackc_diagnostic_severity severity,
    jackc_diagnostic_code code,
    jackc_span span
);
void jackc_diag_emit(const jackc_diag_builder* builder);
void jackc_diag_push(jackc_diagnostic_engine* engine, jackc_diagnostic diagnostic);

#endif
