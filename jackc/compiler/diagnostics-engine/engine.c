#include "engine.h"
#include "compiler/diagnostics-engine/diagnostic.h"
#include "compiler/diagnostics-engine/translations/translation.h"
#include "compiler/lexer/compiler_lexer.h"
#include "core/asserts/jackc_assert.h"
#include "jackc_stdio.h"
#include "jackc_string.h"

jackc_diagnostic_engine jackc_diag_engine_init(
    jackc_string source,
    const char* filename,
    const jackc_diagnostic_translation* translations,
    int output_fd
) {
    jackc_diagnostic_engine engine = {
        .source = source,
        .filename = filename,
        .translations = translations,
        .output_fd = output_fd,
        .size = 0,
        .overflow = false
    };
    return engine;
}

void jackc_diag_engine_reset(
    jackc_diagnostic_engine* engine,
    jackc_string source,
    const char* filename,
    int output_fd
) {
    jackc_assert(engine && "Engine is null");

    engine->source = source;
    engine->filename = filename;
    engine->output_fd = output_fd;
    engine->size = 0;
    engine->overflow = false;
}

static char* token_type_to_str(jack_token_type token) {
    switch (token) {
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_CLASS:
            return "class";
        case TOKEN_CONSTRUCTOR:
            return "constructor";
        case TOKEN_FUNCTION:
            return "function";
        case TOKEN_METHOD:
            return "method";
        case TOKEN_FIELD:
            return "field";
        case TOKEN_STATIC:
            return "static";
        case TOKEN_VAR:
            return "var";
        case TOKEN_INT:
            return "int";
        case TOKEN_CHAR:
            return "char";
        case TOKEN_BOOLEAN:
            return "boolean";
        case TOKEN_VOID:
            return "void";
        case TOKEN_TRUE:
            return "true";
        case TOKEN_FALSE:
            return "false";
        case TOKEN_NULL:
            return "null";
        case TOKEN_THIS:
            return "this";
        case TOKEN_LET:
            return "let";
        case TOKEN_DO:
            return "do";
        case TOKEN_IF:
            return "if";
        case TOKEN_ELSE:
            return "else";
        case TOKEN_WHILE:
            return "while";
        case TOKEN_RETURN:
            return "return";
        case TOKEN_INT_LITERAL:
            return "<integer literal>";
        case TOKEN_STR_LITERAL:
            return "\"string literal\"";
        case TOKEN_IDENTIFIER:
            return "<identifier>";
    }
    return nullptr;
}

static const char* token_to_str(int32_t token) {
    if (token < 256) {
        static char buf[2];
        buf[0] = (char)token;
        buf[1] = '\0';
        return buf;
    }
    const char* str = token_type_to_str((jack_token_type)token);
    if (!str) {
        return "unknown";
    }
    return str;
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

    l = l ? l - 1 : 0;
    return (jack_location){ .line = (uint32_t)l, .col = (uint32_t)(pos - map[l]) };
}

static void build_source_map(const jackc_string source, uint32_t line_starts[], uint32_t lines_total) {
    size_t line = 1;
    for (size_t pos = 0; pos < source.length; ++pos) {
        // Parser exited before consuming all lexer tokens
        // Not all source lines are parsed, so the map is incomplete
        if (line >= lines_total) return;
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

static void print_prefix(int fd, uint32_t max_line_length, bool endl) {
    for (uint32_t i = 0; i < max_line_length + 1; i++) {
        jackc_fprintf(fd, " ");
    }
    jackc_fprintf(fd, "|");
    if (!endl) jackc_fprintf(fd, " ");
    else jackc_fprintf(fd, "\n");
}

static void print_prefix_with_line(int fd, uint32_t line, uint32_t max_line_length) {
    uint32_t spaces_to_print = max_line_length - int_length(line) + 1;
    for (uint32_t i = 0; i < spaces_to_print / 2; i++) {
        jackc_fprintf(fd, " ");
    }
    jackc_fprintf(fd, "%d", line);
    for (uint32_t i = 0; i < spaces_to_print - (spaces_to_print / 2); i++) {
        jackc_fprintf(fd, " ");
    }
    jackc_fprintf(fd, "| ");
}

#define PUTCHAR(c) jackc_fprintf(engine->output_fd, "%c", c)

static char* diagnostic_severity_str(jackc_diagnostic_severity severity) {
    switch (severity) {
        case DIAG_ERROR:
            return "error";
        case DIAG_WARNING:
            return "warning";
        case DIAG_NOTE:
            return "note";
    }

    jackc_assert(false);
    return "";
}

void jackc_diagnostic_engine_report(jackc_diagnostic_engine* engine, uint32_t lines_total) {
    uint32_t line_starts[lines_total];
    line_starts[0] = 0;
    build_source_map(engine->source, line_starts, lines_total);

    uint8_t max_line_length = int_length(lines_total);

    for (size_t i = 0; i < engine->size; i++) {
        jackc_diagnostic diagnostic = engine->diagnostics[i];
        jackc_span span = diagnostic.span;
        jack_location loc = source_map_resolve(line_starts, lines_total, span.start);

        char* severity_str = diagnostic_severity_str(diagnostic.severity);
        jackc_fprintf(engine->output_fd, "%s: ", severity_str);

        jackc_diagnostic_translation translation = engine->translations[diagnostic.code];
        switch (diagnostic.code) {
            case DIAG_UNEXPECTED_TOKEN:
                jackc_fprintf(
                    engine->output_fd,
                    translation.fmt,
                    token_to_str(diagnostic.data.unexpected_token.expected),
                    diagnostic.data.unexpected_token.got.length,
                    diagnostic.data.unexpected_token.got.data
                );
                break;
            case DIAG_INVALID_TOKEN_CLASS_BODY:
            case DIAG_INVALID_TOKEN_TERM:
                jackc_fprintf(
                    engine->output_fd,
                    translation.fmt,
                    diagnostic.data.invalid_token.got.length,
                    diagnostic.data.invalid_token.got.data
                );
                break;
            case DIAG_MISSING_SEMICOLON:
                // Move the ^ to the end of the span, where the semicolon was expected
                loc.col += (span.end - span.start);
                span.start = span.end;
                jackc_fprintf(engine->output_fd, translation.fmt);
                break;
            default:
                jackc_fprintf(engine->output_fd, translation.fmt);
                break;
        }
        jackc_fprintf(engine->output_fd, "\n --> %s:%d:%d\n", engine->filename, loc.line + 1, loc.col + 1);

        print_prefix(engine->output_fd, max_line_length, true);
        print_prefix_with_line(engine->output_fd, loc.line + 1, max_line_length);

        size_t cur_pos = line_starts[loc.line];
        while (
            cur_pos < engine->source.length
            && engine->source.data[cur_pos] != '\r'
            && engine->source.data[cur_pos] != '\n'
        ) {
            PUTCHAR(engine->source.data[cur_pos]);
            ++cur_pos;
        }

        PUTCHAR('\n');
        print_prefix(engine->output_fd, max_line_length, false);
        for (uint32_t col = 0; col < loc.col; ++col) {
            PUTCHAR(' ');
        }
        PUTCHAR('^');
        for (uint32_t span_idx = span.start + 1; span_idx < span.end; ++span_idx) {
            PUTCHAR('~');
        }

        if (translation.desc) {
            jackc_fprintf(engine->output_fd, "\n%s: %s", diagnostic_severity_str(DIAG_NOTE), translation.desc);
        }
        jackc_fprintf(engine->output_fd, i != engine->size - 1 ? "\n\n" : "\n");
    }

    if (engine->overflow) {
        jackc_fprintf(engine->output_fd, "\n%s: Diagnostic engine overflowed.\n", diagnostic_severity_str(DIAG_WARNING));
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
