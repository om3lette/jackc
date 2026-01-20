#include "jackc_assert.h"
#include "common/exit_code.h"
#include "jackc_stdlib.h"

void jackc_assert_fail(const char* expr, const char* file, int line) {
    jackc_print_string("Assertion error ");
    jackc_print_string(expr);
    jackc_print_string(" failed at ");
    jackc_print_string(file);
    jackc_print_string(": ");
    jackc_print_int(line);
    jackc_print_newline();
    jackc_exit(JACKC_ASSERTION_ERROR);
}
