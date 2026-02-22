#include "compiler_lexer_internal.h"
#include "common/jackc_assert.h"
#include "compiler/lexer/compiler_lexer.h"
#include "jackc_string.h"

const char* jack_lexer_pos(const jackc_lexer* lexer, int32_t offset) {
    jackc_assert(lexer != NULL && "Lexer is NULL");
    const char* out = lexer->buffer.data + lexer->pos + offset;
    jackc_assert(out >= lexer->buffer.data && out <= lexer->buffer.data + lexer->buffer.length && "Index out of range");
    // TODO: Assert valid range
    // TODO: Allow negative offset?
    return lexer->buffer.data + lexer->pos + offset;
}

inline const char* jack_lexer_cur_pos(const jackc_lexer* lexer) {
    return jack_lexer_pos(lexer, -1);
}

void jack_lexer_read_char(jackc_lexer* lexer) {
    jackc_assert(lexer != NULL && "Lexer is NULL");
    // String length does not include null terminator
    jackc_assert(lexer->pos <= lexer->buffer.length && "Index out of range");

    lexer->c = lexer->buffer.data[lexer->pos++];
}

[[nodiscard]] bool jack_lexer_read_and_expect(jackc_lexer* lexer, char expected) {
    jackc_assert(lexer != NULL && "Lexer is NULL");

    jack_lexer_read_char(lexer);
    bool is_expected = lexer->c == expected;
    // Restore the state if the char does not match
    lexer->pos -= !is_expected;
    return is_expected;
}

jack_token jack_lexer_new_str_token(jackc_lexer* lexer, int32_t type, const char* start) {
    jackc_assert(lexer != NULL && "Lexer is NULL");
    jackc_assert(start <= jack_lexer_cur_pos(lexer) && "Token start is after current position");

    uint32_t length = (uint32_t)(jack_lexer_cur_pos(lexer) - start);

    jack_token token;
    token.type = type;
    token.loc.line = lexer->line;
    token.loc.col = lexer->col - length;
    token.str = jackc_string_create(start, length);
    return token;
}

jack_token jack_lexer_new_int_token(jackc_lexer* lexer, const char* start, jack_int value) {
    jackc_assert(lexer != NULL && "Lexer is NULL");

    jack_token token = jack_lexer_new_str_token(lexer, TOKEN_NUMBER_INT, start);
    token.value.integer = value;

    return token;
}

void jack_lexer_skip_blank_and_comments(jackc_lexer* lexer) {
    jackc_assert(lexer != NULL && "Lexer is NULL");

    bool is_one_line_comment = false;
    bool is_multiline_comment = false;

    for (;; jack_lexer_read_char(lexer)) {
        if (lexer->c == ' ' || lexer->c == '\t') continue;

        if (
            lexer->c == '\n'
            ||(lexer->c == '\r' && jack_lexer_read_and_expect(lexer, '\n'))
        ) {
            ++lexer->line;
            is_one_line_comment = false;
        }
        else if (lexer->c == '/') {
            is_one_line_comment = jack_lexer_read_and_expect(lexer, '/');
            // "// /*" should not be treated as a start of a multiline comment
            is_multiline_comment = jack_lexer_read_and_expect(lexer, '*') && !is_one_line_comment;
        }
        else if (lexer->c == '*' && jack_lexer_read_and_expect(lexer, '/')) {
            is_multiline_comment = false;
        }
        else if (!is_one_line_comment && !is_multiline_comment) return;

        jackc_assert(is_one_line_comment || is_multiline_comment);
    }
}
