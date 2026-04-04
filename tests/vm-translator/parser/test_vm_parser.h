#ifndef TEST_VM_PARSER_H
#define TEST_VM_PARSER_H

#include "vm-translator/parser/vm_parser.h"

#define REQUIRE_0ARG_INSTRUCTION(_i, _cmd) do { \
    REQUIRE_EQ(_i.cmd, _cmd); \
} while (0)

#define REQUIRE_1ARG_INSTRUCTION(_i, _cmd, _arg1_str) do { \
    REQUIRE_0ARG_INSTRUCTION(_i, _cmd); \
    REQUIRE(jackc_streq(&_i.arg1.str, _arg1_str)); \
} while (0)

#define REQUIRE_1ARG_SEGMENT_INSTRUCTION(_i, _cmd, _segment) do { \
    REQUIRE_0ARG_INSTRUCTION(_i, _cmd); \
    REQUIRE_EQ(_i.arg1.segment, _segment); \
} while (0)

#define REQUIRE_2ARG_INSTRUCTION(_i, _cmd, _arg1_str, _arg2_value) do { \
    REQUIRE_1ARG_INSTRUCTION(_i, _cmd, _arg1_str); \
    CHECK_EQ(_i.arg2.value, _arg2_value); \
} while (0)

#define REQUIRE_2ARG_SEGMENT_INSTRUCTION(_i, _cmd, _segment, _arg2_value) do { \
    REQUIRE_1ARG_SEGMENT_INSTRUCTION(_i, _cmd, _segment); \
    CHECK_EQ(_i.arg2.value, _arg2_value); \
} while (0)

#define REQUIRE_STATUS(_parser, _expected_status) do {\
    REQUIRE_EQ(_parser.status, _expected_status); \
} while (0)

typedef struct {
    const char* buffer;
    vm_cmd cmd;
} no_args_test;

typedef struct {
    const char* buffer;
    vm_cmd cmd;
    const char* arg1_str;
} one_arg_test;

typedef struct {
    const char* buffer;
    vm_cmd cmd;
    const char* arg1_str;
    int32_t arg2_value;
} two_args_test;

typedef struct {
    const char* buffer;
    vm_cmd cmd;
    vm_segment segment;
    int32_t arg2_value;
} push_pop_test;

#endif
