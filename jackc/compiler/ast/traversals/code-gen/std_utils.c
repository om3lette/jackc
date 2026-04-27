#include "std_utils.h"
#include "vm-translator/parser/vm_parser.h"
#include "code_gen_utils.h"
#include "core/asserts/jackc_assert.h"
#include "std/jackc_stdio.h"

static std_subroutine std_subroutines[] = {
    [STD_STRING_NEW] = { "new", 1 },
    [STD_STRING_APPEND_CHAR] = { "appendChar", 2 },
    [STD_MEMORY_ALLOC] = { "alloc", 1 },
};
static_assert(sizeof(std_subroutines) / sizeof(std_subroutine) == NUMBER_OF_STD_FUNCTIONS);

void emit_std_call(FD fd, std_subroutine_call sub) {
    char* class_name = nullptr;

    // Prepare the call by pushing the arguments
    switch (sub.kind) {
        case STD_STRING_NEW:
            vm_emit_push(fd, SEGMENT_CONSTANT, sub.string_new.length);
            class_name = STD_STRING_CLASS;
            break;
        case STD_STRING_APPEND_CHAR:
            vm_emit_push(fd, SEGMENT_CONSTANT, sub.string_append_char.c);
            vm_emit_push(fd, SEGMENT_TEMP, 0);
            class_name = STD_STRING_CLASS;
            break;
        case STD_MEMORY_ALLOC:
            vm_emit_push(fd, SEGMENT_CONSTANT, sub.memory_alloc.words_to_allocate);
            class_name = STD_MEMORY_CLASS;
            break;
        case NUMBER_OF_STD_FUNCTIONS:
            jackc_assert(false && "Helper value is an invalid subroutine");
            return;
    }

    std_subroutine info = std_subroutines[sub.kind];
    jackc_fprintf(fd, "call %s.%s %d\n", class_name, info.name, info.n_args);
}
