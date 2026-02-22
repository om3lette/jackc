#include "compiler_lexer.h"
#include "common/jackc_assert.h"
#include "compiler/lexer/compiler_lexer_internal.h"
#include "compiler/lexer/compiler_reserved_keywords.h"
#include "jackc_stdlib.h"
#include "jackc_string.h"

jackc_lexer* jack_lexer_init(const char* buffer) {
    jackc_assert(buffer != NULL && "Lexer's buffer cannot be NULL");

    jackc_lexer* lexer = jackc_alloc(sizeof(jackc_lexer));
    lexer->buffer = jackc_string_create(buffer, jackc_strlen(buffer));

    lexer->c = LEXER_DEFAULT_CHAR;
    lexer->line = LEXER_DEFAULT_LINE;
    lexer->col = LEXER_DEFAULT_COL;
    lexer->pos = 0;
    return lexer;
}

jack_token jack_lexer_next_token(jackc_lexer* lexer) {
    jackc_assert(lexer != NULL && "Lexer is NULL");

    jack_lexer_skip_blank_and_comments(lexer);

    // Numbers
    if (lexer->c >= '0' && lexer->c <= '9') {
        const char* num_start = jack_lexer_cur_pos(lexer);
        jack_int value = 0;
        do {
            value = value * 10 + lexer->c - '0';
            jack_lexer_read_char(lexer);
        } while((int8_t)lexer->c >= 0 && lexer->c <= '9');
        return jack_lexer_new_int_token(lexer, num_start, value);
    }

    // Identifiers
    char c = jackc_tolower(lexer->c);
    if (c >= 'a' && c <= 'z') {
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
        return jack_lexer_new_str_token(lexer, c, jack_lexer_cur_pos(lexer));
    }
    jackc_assert(c != '\n' && c != '\r' && c != '\0' && "These chars are not meant to be processed by this code block");

    c = lexer->c;
    jack_lexer_read_char(lexer);
    return jack_lexer_new_str_token(lexer, c, jack_lexer_cur_pos(lexer) - 1);
}
