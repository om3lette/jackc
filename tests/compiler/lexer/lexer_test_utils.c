#include "lexer_test_utils.h"
#include "common/jackc_assert.h"
#include "compiler/lexer/compiler_lexer.h"
#include "jackc_string.h"

bool is_token_equal_str(const jack_token* token, int token_type, const char* token_value) {
    return token->type == token_type && jackc_streq(&token->str, token_value);
}

bool is_token_equal_int(const jack_token* token, jack_int token_value) {
    return token->type == TOKEN_NUMBER_INT && token->value.integer == token_value;
}

bool is_token_match(const jack_token* token, char chr) {
    if (token->type == '\0') return token->str.length == 0 && token->str.data[0] == chr;
    return token->type == chr && token->str.length == 1 && token->str.data[0] == chr;
}

void test_jack_lexer_new_buffer(jackc_lexer* lexer, const char* buffer) {
    jackc_assert(lexer != NULL && "Lexer is NULL");
    jackc_assert(buffer != NULL && "Buffer is NULL");

    lexer->buffer = jackc_string_create(buffer, jackc_strlen(buffer));
    lexer->c = ' ';
    lexer->line = 1;
    lexer->col = 1;
    lexer->pos = 0;
}
