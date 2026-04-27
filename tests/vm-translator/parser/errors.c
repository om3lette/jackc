#include "tau.h"
#include "test_vm_parser.h"
#include "vm-translator/parser/vm_parser.h"

static vm_parser init_parser_and_parse_instruction(const char* buffer) {
    vm_parser parser = vm_parser_init(&jackc_string_from_str(buffer));
    return parser;
}

#define REGISTER_TEST(_name, _buffer, _expected_status)                \
    TEST(vm_parser, error_handling_##_name) {                          \
        vm_parser parser = init_parser_and_parse_instruction(_buffer); \
        REQUIRE_STATUS(parser, _expected_status);                      \
    }

REGISTER_TEST(invalid_instruction_type, "random label 2", VM_INVALID_CMD)
REGISTER_TEST(invalid_segment, "push ladder 2", VM_INVALID_SEGMENT)
REGISTER_TEST(empty_first_argument, "label", VM_EMPTY_FIRST_ARGUMENT)
REGISTER_TEST(empty_second_argument, "call Math.init", VM_EMPTY_SECOND_ARGUMENT)
REGISTER_TEST(pop_segment_const, "pop constant 2", VM_POP_SEGMENT_CONST)
REGISTER_TEST(invalid_pointer_idx, "push pointer 42", VM_INVALID_POINTER_IDX)
REGISTER_TEST(invalid_second_arg, "push constant -null", VM_INVALID_ARG_2)
