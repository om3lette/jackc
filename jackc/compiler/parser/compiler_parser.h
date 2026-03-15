#ifndef JACKC_COMPILER_PARSER_H
#define JACKC_COMPILER_PARSER_H

#include "compiler/lexer/compiler_lexer.h"

typedef struct {
    jackc_lexer lexer;
    jack_token current;
    jack_token next;
} jackc_parser;

#endif
