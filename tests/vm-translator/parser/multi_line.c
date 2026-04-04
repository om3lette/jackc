#include "tau.h"
#include "test_vm_parser.h"
#include "vm-translator/parser/vm_parser.h"

static vm_parser init_parser(const char* buffer) {
    return jackc_parser_init(&jackc_string_from_str(buffer));
}

TEST(vm_parser, multi_line_lf) {
    vm_parser parser = init_parser("sub\nadd\nneg");
    CHECK_0ARG_INSTRUCTION(parser.current, C_SUB);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_ADD);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_NEG);
    REQUIRE_FALSE(vm_parser_has_more_lines(&parser));
}

TEST(vm_parser, multi_line_crlf) {
    vm_parser parser = init_parser("sub\r\nadd\r\nneg");
    CHECK_0ARG_INSTRUCTION(parser.current, C_SUB);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_ADD);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_NEG);
    REQUIRE_FALSE(vm_parser_has_more_lines(&parser));
}

TEST(vm_parser, multi_line_with_comments_at_the_start) {
    vm_parser parser = init_parser("//test comment\nsub\n// more connents \nadd\nneg");
    CHECK_0ARG_INSTRUCTION(parser.current, C_SUB);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_ADD);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_NEG);
    REQUIRE_FALSE(vm_parser_has_more_lines(&parser));
}

TEST(vm_parser, multi_line_with_comments) {
    vm_parser parser = init_parser("//test comment\n\tsub//at the EOL!\n// more connents \nadd\nneg");
    CHECK_0ARG_INSTRUCTION(parser.current, C_SUB);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_ADD);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_NEG);
    REQUIRE_FALSE(vm_parser_has_more_lines(&parser));
}

TEST(vm_parser, multi_line_with_spacing_at_the_beggining) {
    vm_parser parser = init_parser("\t  \t//test comment\n\tsub\nneg");
    CHECK_0ARG_INSTRUCTION(parser.current, C_SUB);
    vm_parser_advance(&parser);
    CHECK_0ARG_INSTRUCTION(parser.current, C_NEG);
    REQUIRE_FALSE(vm_parser_has_more_lines(&parser));
}
