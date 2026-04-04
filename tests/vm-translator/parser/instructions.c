#include "test_vm_parser.h"
#include "tau.h"
#include "vm-translator/parser/vm_parser.h"
#include "vm-translator/parser/vm_parser_internal.h"

static vm_line init_parser_and_parse_instruction(const char* buffer) {
    vm_parser parser = jackc_parser_init(&jackc_string_from_str(buffer));
    return vm_parser_parse_line(&parser);
}

static no_args_test no_args_instructions[] = {
    { "add", C_ADD }, { "sub", C_SUB },
    { "mul", C_MUL }, { "div", C_DIV },
    { "neg", C_NEG }, { "eq",  C_EQ  },
    { "gt",  C_GT  }, { "lt",  C_LT  },
    { "and", C_AND }, { "or",  C_OR  },
    { "not", C_NOT }, { "return", C_RETURN }
};
TEST(vm_parser, instructions_0_args) {
    for (uint32_t i = 0; i < sizeof(no_args_instructions) / sizeof(no_args_instructions[0]); ++i) {
        no_args_test test = no_args_instructions[i];
        vm_line line = init_parser_and_parse_instruction(test.buffer);
        REQUIRE_0ARG_INSTRUCTION(line, test.cmd);
    }
}

static one_arg_test one_arg_tests[] = {
    { "label IF_TRUE",   C_LABEL,   "IF_TRUE" }, { "label IF_FALSE_0",   C_LABEL,   "IF_FALSE_0" }, // Label with numbers
    { "goto  IF_TRUE",   C_GOTO,    "IF_TRUE" }, { "goto  IF_FALSE_0",   C_GOTO,    "IF_FALSE_0" },
    { "if-goto IF_TRUE", C_IF_GOTO, "IF_TRUE" }, { "if-goto IF_FALSE_0", C_IF_GOTO, "IF_FALSE_0" }
};
TEST(vm_parser, instructions_1_arg) {
    for (uint32_t i = 0; i < sizeof(one_arg_tests) / sizeof(one_arg_tests[0]); ++i) {
        one_arg_test test = one_arg_tests[i];
        vm_line line = init_parser_and_parse_instruction(test.buffer);
        REQUIRE_1ARG_INSTRUCTION(line, test.cmd, test.arg1_str);
    }
}

static push_pop_test push_pop_tests[] = {
    { "push this 42",   C_PUSH, SEGMENT_THIS,   42 }, { "push that -42",     C_PUSH, SEGMENT_THAT,     -42 },
    { "push local 41",  C_PUSH, SEGMENT_LOCAL,  41 }, { "push constant -41", C_PUSH, SEGMENT_CONSTANT, -41 },
    { "push static 9",  C_PUSH, SEGMENT_STATIC,  9 }, { "push temp -9",      C_PUSH, SEGMENT_TEMP,     -9  },
    { "push pointer 0", C_PUSH, SEGMENT_POINTER, 0 }, { "push argument 0",   C_PUSH, SEGMENT_ARG,       0  },

    { "pop this 42",    C_POP,  SEGMENT_THIS,   42 }, { "pop that -42",      C_POP,  SEGMENT_THAT,     -42 },
    { "pop local 41",   C_POP,  SEGMENT_LOCAL,  41 },
    { "pop static 9",   C_POP,  SEGMENT_STATIC,  9 }, { "pop temp -9",       C_POP,  SEGMENT_TEMP,     -9  },
    { "pop pointer 0",  C_POP,  SEGMENT_POINTER, 0 }, { "pop argument 0",    C_POP,  SEGMENT_ARG,      -0  }
};
TEST(vm_parser, instructions_push_pop_tests) {
    for (uint32_t i = 0; i < sizeof(push_pop_tests) / sizeof(push_pop_tests[0]); ++i) {
        push_pop_test test = push_pop_tests[i];
        vm_line line = init_parser_and_parse_instruction(test.buffer);
        REQUIRE_2ARG_SEGMENT_INSTRUCTION(line, test.cmd, test.segment, test.arg2_value);
    }
}

static two_args_test two_args_tests[]  = {
    { "function Sys.init 42", C_FUNCTION, "Sys.init", 42 },
    { "call Sys.init 128", C_CALL, "Sys.init", 128 }
};
TEST(vm_parser_arg1, instructions_2_args) {
    for (uint32_t i = 0; i < sizeof(two_args_tests) / sizeof(two_args_tests[0]); ++i) {
        two_args_test test = two_args_tests[i];
        vm_line line = init_parser_and_parse_instruction(test.buffer);
        REQUIRE_2ARG_INSTRUCTION(line, test.cmd, test.arg1_str, test.arg2_value);
    }
}
