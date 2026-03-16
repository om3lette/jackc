#include "jackc_assert.h"
#include "core/exit_code.h"
#include "jackc_stdlib.h"
#include "core/logging/logger.h"

void jackc_assert_fail(const char* expr, const char* file, int line) {
    LOG_FATAL("%s\n", expr);
    LOG_FATAL("At %s:%d\n", file, line); // Print a "failed" banner
    jackc_exit(JACKC_ASSERTION_ERROR);
}
