#include "compiler_lexer.h"
#include "core/asserts/jackc_assert.h"
#include "compiler/lexer/compiler_lexer_internal.h"
#include "compiler/lexer/compiler_reserved_keywords.h"
#include "jackc_string.h"

static inline jack_token eof_token(jack_lexer* lexer) {
    return jack_lexer_new_str_token(lexer, '\0', jack_lexer_cur_pos(lexer));
}

static inline bool is_eol(char c) {
    return c == '\n' || c == '\r' || c == '\0';
}

jack_lexer jack_lexer_init(const char* buffer) {
    jackc_assert(buffer != NULL && "Lexer's buffer cannot be NULL");

    jack_lexer lexer;
    lexer.buffer = jackc_string_create(buffer, jackc_strlen(buffer));

    lexer.c = LEXER_DEFAULT_CHAR;
    lexer.line = LEXER_DEFAULT_LINE;
    lexer.pos = 0;
    return lexer;
}

jack_token jack_lexer_next_token(jack_lexer* lexer) {
    jackc_assert(lexer != NULL && "Lexer is NULL");

    jack_lexer_skip_blank_and_comments(lexer);

    // Numbers
    if (lexer->c >= '0' && lexer->c <= '9') {
        const char* num_start = jack_lexer_cur_pos(lexer);
        jack_int value = 0;
        do {
            value = value * 10 + lexer->c - '0';
            jack_lexer_read_char(lexer);
        } while(lexer->c >= '0' && lexer->c <= '9' && !is_eol(lexer->c));
        return jack_lexer_new_int_token(lexer, num_start, value);
    }

    if (lexer->c == '"') {
        const char* str_start = jack_lexer_cur_pos(lexer) + 1;
        jack_lexer_read_char(lexer);
        // TODO: Get "is_end_of_line" function
        while (lexer->c != '"' && !is_eol(lexer->c)) {
            jack_lexer_read_char(lexer);
        }
        jack_token token = jack_lexer_new_str_token(lexer, TOKEN_STR_LITERAL, str_start);
        if (lexer->c == '"') {
            jack_lexer_read_char(lexer);
        }
        return token;
    }

    // Identifiers
    char c = jackc_tolower(lexer->c);
    if ((c >= 'a' && c <= 'z') || c == '_') {
        // Lexer is pointing to the char after 'c'. Subtract 1 to get the start position
        const char* start_ptr = jack_lexer_cur_pos(lexer);
        do {
            jack_lexer_read_char(lexer);
            c = jackc_tolower(lexer->c);
        } while ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');

        // TODO: Make a separate function?
        size_t length = (size_t)(jack_lexer_cur_pos(lexer) - start_ptr);
        return jack_lexer_new_str_token(lexer, (int32_t)jack_lexer_lookup_keyword(start_ptr, length), start_ptr);
    }

    if (c == '\0') {
        // '\0' does not follow common logic as there is no char after it
        return eof_token(lexer);
    }
    jackc_assert(c != '\n' && c != '\r' && c != '\0' && "These chars are not meant to be processed by this code block");

    c = lexer->c;
    jack_lexer_read_char(lexer);
    return jack_lexer_new_str_token(lexer, c, jack_lexer_cur_pos(lexer) - 1);
}

bool jack_lexer_has_token(jack_lexer* lexer) {
    return lexer->c != '\0';
}
