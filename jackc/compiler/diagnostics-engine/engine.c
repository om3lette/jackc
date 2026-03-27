#include "engine.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/asserts/jackc_assert.h"
#include "jackc_stdlib.h"

jackc_diagnostic_engine jack_diag_engine_init(jackc_string source, const char* filename, const jackc_diagnostic_translation* translations) {
    jackc_diagnostic_engine engine = {
        .source = source,
        .filename = filename,
        .translations = translations,
        .size = 0,
        .overflow = false
    };
    return engine;
}

void jackc_diag_push(jackc_diagnostic_engine* engine, jackc_diagnostic diagnostic) {
    if (engine->size < MAX_DIAGNOSTICS) {
        engine->diagnostics[engine->size++] = diagnostic;
    } else {
        engine->overflow = true;
    }
}

static jack_location source_map_resolve(
    uint32_t* map,
    uint32_t lines_total,
    size_t pos
) {
    size_t l = 0, r = lines_total - 1;
    while (l < r) {
        size_t m = l + (r - l) / 2;
        if (map[m] <= pos) {
            l = m + 1;
        } else {
            r = m;
        }
    }
    return (jack_location){ .line = (uint32_t)l, .col = (uint32_t)(pos - map[l]) };
}

static void build_source_map(const jackc_string source, uint32_t line_starts[]) {
    size_t line = 1;
    for (size_t pos = 0; pos < source.length; ++pos) {
        if (source.data[pos] == '\n') {
            line_starts[line++] = (uint32_t)(pos + 1);
        }
    }
}

static uint8_t int_length(uint32_t n) {
    uint8_t length = 0;
    while (n) {
        ++length;
        n /= 10;
    }
    return length;
}

static void print_prefix(uint32_t max_line_length, bool endl) {
    for (uint32_t i = 0; i < max_line_length + 1; i++) {
        jackc_putchar(' ');
    }
    jackc_putchar('|');
    if (!endl) jackc_putchar(' ');
    else jackc_putchar('\n');
}

static void print_prefix_with_line(uint32_t line, uint32_t max_line_length) {
    uint32_t spaces_to_print = max_line_length - int_length(line) + 1;
    for (uint32_t i = 0; i < spaces_to_print / 2; i++) {
        jackc_putchar(' ');
    }
    jackc_printf("%d", line);
    for (uint32_t i = 0; i < spaces_to_print - (spaces_to_print / 2); i++) {
        jackc_putchar(' ');
    }
    jackc_putchar('|');
    jackc_putchar(' ');
}

void jackc_diagnostic_engine_report(jackc_diagnostic_engine* engine, uint32_t lines_total) {
    uint32_t line_starts[lines_total];
    line_starts[0] = 0;
    build_source_map(engine->source, line_starts);

    uint8_t max_line_length = int_length(lines_total);

    for (size_t i = 0; i < engine->size; i++) {
        jackc_diagnostic diagnostic = engine->diagnostics[i];
        jackc_span span = diagnostic.span;
        jack_location loc = source_map_resolve(line_starts, lines_total, span.start);
        const char* line_ptr = engine->source.data + line_starts[loc.line];

        char* severity_str = "";
        switch (diagnostic.severity) {
            case DIAG_ERROR:
                severity_str = "error";
                break;
            case DIAG_WARNING:
                severity_str = "warning";
                break;
            case DIAG_NOTE:
                severity_str = "note";
                break;
        }

        // TODO: Unique formatting per diagnostic code
        switch (diagnostic.code) {
            case DIAG_UNEXPECTED_TOKEN:
                break;
            case DIAG_MISSING_VARIABLE_KIND:
                break;
            case DIAG_MISSING_VARIABLE_NAME:
                break;
            case DIAG_INVALID_VARIABLE_TYPE:
                break;
            case DIAG_INVALID_RETURN_TYPE:
                break;
        }

        jackc_printf("%s: %s\n", severity_str, engine->translations[diagnostic.code].fmt);
        jackc_printf(" --> %s:%d:%d\n", engine->filename, loc.line + 1, loc.col + 1);

        print_prefix(max_line_length, true);
        print_prefix_with_line(loc.line + 1, max_line_length);

        while (*line_ptr != '\r' && *line_ptr != '\n' && *line_ptr != '\0') {
            jackc_putchar(*line_ptr);
            ++line_ptr;
        }
        jackc_putchar('\n');
        print_prefix(max_line_length, false);
        for (uint32_t col = 0; col < loc.col; ++col) {
            jackc_putchar(' ');
        }
        jackc_putchar('^');
        for (uint32_t span_idx = span.start + 1; span_idx < span.end; ++span_idx) {
            jackc_putchar('~');
        }
        jackc_putchar('\n');
    }
}

jackc_diag_builder jackc_diag_begin(
    jackc_diagnostic_engine* engine,
    jackc_diagnostic_severity severity,
    jackc_diagnostic_code code,
    jackc_span span
) {
    return (jackc_diag_builder){
        .engine = engine,
        .diag = {
            .severity = severity,
            .code = code,
            .span = span
        }
    };
}

void jackc_diag_emit(const jackc_diag_builder* builder) {
    jackc_assert(builder && "Builder is null");
    jackc_diag_push(builder->engine, builder->diag);
}
