#include "jackc_stdlib.h"
#include "rars/rars_syscalls.h"

void jackc_exit(int code) {
    rars_exit2(code);
}
