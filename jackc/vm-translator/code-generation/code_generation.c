#include "code_gen.h"
#include "common/jackc_assert.h"
#include "jackc_stdio.h"
#include "jackc_string.h"
#include "vm-translator/code-generation/utils.h"
#include "vm-translator/constants.h"
#include "vm-translator/parser.h"
#include <stdint.h>

void jackc_vm_code_bootstrap(vm_code_generator* generator) {
    // TODO: Replace with strcmp
    if (JACK_SP_REG[0] == 's' && JACK_SP_REG[1] == 'p') return;
    VM_CODE_GEN_HELP_COMMENT_TAB(generator->fd, "Bootstrap code", 0);
    jackc_fprintf(
        generator->fd,
        "mv %s, sp\n",
        "\n"
        JACK_SP_REG
    );
}

/**
 * @todo inline?
 */
void vm_code_gen_load_value_from(int fd, jackc_vm_segment_type segment, char* dest_reg, int offset) {
    jackc_assert(offset >= 0 && "Offset must be >= 0");

    long byte_offset = word_to_bytes(offset);
    switch (segment) {
        case SEGMENT_THIS:
            jackc_fprintf(
                fd,
                "\tlw %s, -%d(%s)\n"
                "\tlw %s, 0(%s)\n",
                LOAD_REG, byte_offset, SEGMENT_THIS_REG,
                dest_reg, LOAD_REG
            );
            break;
        case SEGMENT_THAT:
            jackc_fprintf(
                fd,
                "\tlw %s, -%d(%s)\n"
                "\tlw %s, 0(%s)\n",
                LOAD_REG, byte_offset, SEGMENT_THAT_REG,
                dest_reg, LOAD_REG
            );
            break;
        case SEGMENT_POINTER:
            char* segment_reg = offset == POINTER_THIS ? SEGMENT_THIS_REG : SEGMENT_THAT_REG;
            jackc_fprintf(fd, "\tlw %s, -%d(%s)\n", dest_reg, byte_offset, segment_reg);
            break;
        case SEGMENT_STATIC:
            jackc_fprintf(fd, "\tla %s, %s\n", dest_reg, vm_code_gen_generate_static_name((uint32_t)offset));
            break;
        default:
            jackc_assert(false && "Provided segment type cannot be loaded");
            break;
    }
}

void vm_code_gen_stack_alloc(int fd, int words) {
    jackc_fprintf(fd, "\taddi %s, %s, -%d\n", JACK_SP_REG, JACK_SP_REG, word_to_bytes(words));
}

void vm_code_gen_stack_dealloc(int fd, int words) {
    jackc_fprintf(fd, "\taddi %s, %s, %d\n", JACK_SP_REG, JACK_SP_REG, word_to_bytes(words));
}

void vm_code_gen_push(int fd, jackc_vm_segment_type type, int idx) {
    VM_CODE_GEN_HELP_COMMENT_TAB(fd, "push %s %d\n", vm_segment_type_to_string(type), idx);
    vm_code_gen_stack_alloc(fd, 1);

    switch (type) {
        case SEGMENT_THIS:
            vm_code_gen_load_value_from(fd, SEGMENT_THIS, LOAD_REG, idx);
            jackc_fprintf(fd, "\tsw %s, 0(%s)\n", LOAD_REG, JACK_SP_REG);
            break;
        case SEGMENT_THAT:
            vm_code_gen_load_value_from(fd, SEGMENT_THAT, LOAD_REG, idx);
            jackc_fprintf(fd, "\tsw %s, 0(%s)\n", LOAD_REG, JACK_SP_REG);
            break;
        case SEGMENT_ARG:
            jackc_fprintf(
                fd,
                "\tlw %s, -%d(%s)\n"
                "\tsw %s, 0(%s)\n",
                LOAD_REG, word_to_bytes(idx), SEGMENT_ARG_REG,
                LOAD_REG, JACK_SP_REG
            );
            break;
        case SEGMENT_LOCAL:
            jackc_fprintf(
                fd,
                "\tlw %s, -%d(%s)\n"
                "\tsw %s, 0(%s)\n",
                LOAD_REG, word_to_bytes(idx), SEGMENT_LCL_REG,
                LOAD_REG, JACK_SP_REG
            );
            break;
        case SEGMENT_CONSTANT:
            jackc_fprintf(
                fd,
                "\tli %s, %d\n"
                "\tsw %s, 0(%s)\n",
                LOAD_REG, idx,
                LOAD_REG, JACK_SP_REG
            );
            break;
        case SEGMENT_STATIC:
            vm_code_gen_load_value_from(fd, SEGMENT_STATIC, LOAD_REG, idx);
            jackc_fprintf(fd, "\tsw %s, 0(%s)\n", LOAD_REG, JACK_SP_REG);
            break;
        case SEGMENT_TEMP:
            jackc_assert(idx >= 0 && idx < 7 && "Invalid temp index.");
            jackc_fprintf(fd, "\tsw t%d, 0(%s)\n", idx, JACK_SP_REG);
            break;
        case SEGMENT_POINTER:
            vm_code_gen_load_value_from(fd, SEGMENT_POINTER, LOAD_REG, idx);
            jackc_fprintf(fd, "\tsw %s, 0(%s)\n", LOAD_REG, JACK_SP_REG);
            break;
    }
}

void vm_code_gen_pop(int fd, jackc_vm_segment_type type, int idx) {
    // TODO: Turn off debug mode
    VM_CODE_GEN_HELP_COMMENT_TAB(fd, "pop %s %d\n", vm_segment_type_to_string(type), idx);

    switch (type) {
        case SEGMENT_THIS:
            jackc_fprintf(
                fd,
                "\tlw %s, 0(%s)\n"
                "\tsw %s, %d(%s)\n",
                LOAD_REG, JACK_SP_REG,
                LOAD_REG, word_to_bytes(idx), SEGMENT_THIS_REG
            );
            break;
        case SEGMENT_THAT:
            jackc_fprintf(
                fd,
                "\tlw %s, 0(%s)\n"
                "\tsw %s, %d(%s)\n",
                LOAD_REG, JACK_SP_REG,
                LOAD_REG, word_to_bytes(idx), SEGMENT_THAT_REG
            );
            break;
        case SEGMENT_ARG:
            // TODO: Add support for > 8 arguments
            jackc_assert(idx >= 0 && idx < 8 && "Invalid argument index.");
            jackc_fprintf(fd, "\tlw a%d, 0(%s)\n", idx, JACK_SP_REG);
            break;
        case SEGMENT_LOCAL:
            jackc_fprintf(
                fd,
                "\tlw %s, 0(%s)\n",
                "\tsw %s, %d(%s)\n",
                LOAD_REG, JACK_SP_REG,
                LOAD_REG, word_to_bytes(idx + 1), SEGMENT_LCL_REG
            );
            break;
        case SEGMENT_CONSTANT:
            jackc_assert(false && "pop with constant segment is not allowed");
            break;
        case SEGMENT_STATIC:
            jackc_fprintf(
                fd,
                "\tlw %s, 0(%s)\n",
                "\tsw %s, %s\n"
                LOAD_REG, JACK_SP_REG,
                LOAD_REG, vm_code_gen_generate_static_name((uint32_t)idx)
            );
            break;
        case SEGMENT_TEMP:
            jackc_assert(idx >= 0 && idx < 7 && "Invalid temp index.");
            jackc_fprintf(fd, "\tlw t%d, 0(%s)\n", idx, JACK_SP_REG);
            break;
        case SEGMENT_POINTER:
            char* selected_register = (idx == POINTER_THIS) ? SEGMENT_THIS_REG : SEGMENT_THAT_REG;
            jackc_fprintf(
                fd,
                "\tsw %s, 0(%s)\n"
                "\tmv %s, %s\n",
                LOAD_REG, JACK_SP_REG,
                selected_register, LOAD_REG
            );
            break;
    }

    // Deallocate space on the stack.
    vm_code_gen_stack_dealloc(fd, 1);
}

void vm_code_gen_arithmetic_2_args(int fd, jackc_vm_cmd_type cmd) {
    vm_code_gen_pop(fd, SEGMENT_TEMP, OP_ARG_1_IDX);

    char op[4];
    switch (cmd) {
        case C_NEG:
            jackc_strcpy(op, "neg");
            break;
        case C_NOT:
            jackc_strcpy(op, "not");
            break;
        default:
            jackc_assert(false && "Invalid 2 argument arithmetic command");
            break;
    }
    jackc_fprintf(fd, "\t%s %s, %s\n", op, OP_RES_REG, OP_ARG_1_REG);
    vm_code_gen_push(fd, SEGMENT_TEMP, OP_RES_IDX);
}

/**
 * @todo: align assembly instructions
 */
void vm_code_gen_arithmetic_3_args(int fd, jackc_vm_cmd_type cmd) {
    // perf: allocation can be done in one command
    vm_code_gen_pop(fd, SEGMENT_TEMP, OP_ARG_2_IDX);
    vm_code_gen_pop(fd, SEGMENT_TEMP, OP_ARG_1_IDX);

    char op[4];
    switch (cmd) {
        case C_ADD:
            jackc_strcpy(op, "add");
            break;
        case C_SUB:
            jackc_strcpy(op, "sub");
            break;
        case C_AND:
            jackc_strcpy(op, "and");
            break;
        case C_OR:
            jackc_strcpy(op, "or");
            break;
        default:
            jackc_assert(false && "Invalid 3 argument arithmetic command");
            break;
    }
    jackc_fprintf(fd, "\t%s %s, %s, %s\n", op, OP_RES_REG, OP_ARG_1_REG, OP_ARG_2_REG);
    vm_code_gen_push(fd, SEGMENT_TEMP, OP_RES_IDX);
}

void vm_code_gen_return(int fd) {
    VM_CODE_GEN_HELP_COMMENT_TAB(fd, "Store the return value\n", 0);
    jackc_fprintf(fd, "\tlw %s, 0(%s)\n", RET_REG, JACK_SP_REG);
    VM_CODE_GEN_HELP_COMMENT_TAB(fd, "Restore the frame pointer\n", 0);
    jackc_fprintf(
        fd,
        "\taddi %s, %s, %d\n"
        "\tret\n",
        JACK_SP_REG, SEGMENT_LCL_REG, -word_to_bytes(1)
    );
}

void vm_code_gen_branching(int fd, jackc_vm_cmd_type cmd, const jackc_string* label) {
    switch (cmd) {
        case C_IF_GOTO:
            vm_code_gen_pop(fd, SEGMENT_TEMP, LOAD_IDX);
            jackc_fprintf(
                fd,
                "\tbne %s, x0, %.*s\n",
                LOAD_REG, label->length, label->data
            );
            break;
        case C_GOTO:
            jackc_fprintf(fd, "\tj %.*s\n", label->length, label->data);
            break;
        default:
            jackc_assert(false && "Invalid branching command");
            break;
    }
}

void vm_code_gen_label(int fd, const jackc_string* label) {
    jackc_fprintf(fd, "%.*s:\n", label->length, label->data);
}

char* vm_code_gen_function_label(const jackc_string* name) {
    // It is assumed that the parser has already validated the function name
    jackc_assert(
        name != NULL
        && name->length != 0 && name->length < MAX_FUNCTION_NAME_LENGTH
        && name->data != NULL
        && "Invalid function name input."
    );
    static char label_function_buffer[MAX_FUNCTION_NAME_LENGTH + 32];
    jackc_sprintf(label_function_buffer, "%.*s", name->length, name->data);
    label_function_buffer[name->length] = '\0';
    return label_function_buffer;
}

/**
 * @todo: Add filename/index to function label
 */
void vm_code_gen_function(int fd, const jackc_string* name, int local_cnt) {
    jackc_fprintf(fd, "%s:\n", vm_code_gen_function_label(name));

    /**
     * Example stack layout with local_cnt = 2:
     * [sp+32] argument 0     | <- ARG ptr
     * [sp+28] argument 1     |
     * [sp+24] OLD_LCL        |
     * [sp+20] OLD_ARG        |
     * [sp+16] OLD_THAT       |
     * [sp+12] OLD_THIS       |
     * [sp+ 8] return address |
     * [sp+ 4] local arg 0    | <- LCL ptr
     * [sp+ 0] local arg 1    |
    */
    VM_CODE_GEN_HELP_COMMENT_TAB(fd, "Initialize LCL\n", 0);
    jackc_fprintf(
        fd,
        "\taddi %s, %s, 4\n",
        SEGMENT_LCL_REG, JACK_SP_REG
    );
    if (local_cnt > 0) {
        vm_code_gen_stack_alloc(fd, word_to_bytes(local_cnt));
    }
}

/**
 * Stack layout:
 * [sp+24] argument 0
 * [sp+20] argument 1
 * [sp+16] LCL
 * [sp+12] ARG
 * [sp+ 8] THAT
 * [sp+ 4] THIS
 * [sp+ 0] return address
*/
void vm_code_gen_call(int fd, const jackc_string* function_name, int arg_count) {
    VM_CODE_GEN_HELP_COMMENT_TAB(fd, "Save registers\n", 0);
    const int SAVED_REGISTERS = 5;
    vm_code_gen_stack_alloc(fd, SAVED_REGISTERS);
    jackc_fprintf(
        fd,
        "\tsw ra, 0(%s)\n"
        "\tsw %s, %d(%s)\n"
        "\tsw %s, %d(%s)\n"
        "\tsw %s, %d(%s)\n"
        "\tsw %s, %d(%s)\n",
        JACK_SP_REG,
        SEGMENT_THIS_REG, word_to_bytes(1), JACK_SP_REG,
        SEGMENT_THAT_REG, word_to_bytes(2), JACK_SP_REG,
        SEGMENT_ARG_REG, word_to_bytes(3), JACK_SP_REG,
        SEGMENT_LCL_REG, word_to_bytes(4), JACK_SP_REG
    );
    if (arg_count > 0) {
        VM_CODE_GEN_HELP_COMMENT_TAB(fd, "Set the ARG ptr\n", 0);
        jackc_fprintf(
            fd,
            "\taddi %s, %s, %d\n",
            SEGMENT_ARG_REG, JACK_SP_REG, word_to_bytes(SAVED_REGISTERS + arg_count - 1)
        );
    }

    // Make a function call
    jackc_fprintf(fd, "\tcall %s\n", vm_code_gen_function_label(function_name));

    VM_CODE_GEN_HELP_COMMENT_TAB(fd, "Restore registers\n", 0);
    jackc_fprintf(
        fd,
        "\tlw ra, 0(%s)\n"
        "\tlw %s, %d(%s)\n"
        "\tlw %s, %d(%s)\n"
        "\tlw %s, %d(%s)\n"
        "\tlw %s, %d(%s)\n",
        JACK_SP_REG,
        SEGMENT_THIS_REG, word_to_bytes(1), JACK_SP_REG,
        SEGMENT_THAT_REG, word_to_bytes(2), JACK_SP_REG,
        SEGMENT_ARG_REG, word_to_bytes(3), JACK_SP_REG,
        SEGMENT_LCL_REG, word_to_bytes(4), JACK_SP_REG
   );

   vm_code_gen_stack_dealloc(fd, SAVED_REGISTERS + arg_count - 1);
   // push the return value
   jackc_fprintf(fd, "\tsw %s, 0(%s)\n", RET_REG, JACK_SP_REG);
}

void vm_code_gen_comparisons(int fd, jackc_vm_cmd_type cmd) {
    vm_code_gen_pop(fd, SEGMENT_TEMP, OP_ARG_2_IDX);
    vm_code_gen_pop(fd, SEGMENT_TEMP, OP_ARG_1_IDX);

    switch (cmd) {
        case C_EQ:
            jackc_fprintf(fd,
                "\tsub %s, %s, %s\n"
                "\tseqz %s, %s\n",
                OP_RES_REG, OP_ARG_1_REG, OP_ARG_2_REG,
                OP_RES_REG, OP_RES_REG
            );
            break;
        case C_GT:
            jackc_fprintf(fd, "\tsgt %s, %s, %s\n", OP_RES_REG, OP_ARG_1_REG, OP_ARG_2_REG);
            break;
        case C_LT:
            jackc_fprintf(fd, "\tslt %s, %s, %s\n", OP_RES_REG, OP_ARG_1_REG, OP_ARG_2_REG);
            break;
        default:
            jackc_assert(false && "Invalid comparison command");
            break;
    }
    vm_code_gen_push(fd, SEGMENT_TEMP, OP_RES_IDX);
}

void jackc_vm_code_gen_line(vm_code_generator* generator, const jackc_parser* parser) {
    int fd = generator->fd;
    switch (parser->cmd) {
        case C_ADD:
        case C_SUB:
        case C_AND:
        case C_OR:
            vm_code_gen_arithmetic_3_args(fd, parser->cmd);
            break;
        case C_NEG:
        case C_NOT:
            vm_code_gen_arithmetic_2_args(fd, parser->cmd);
            break;
        case C_EQ:
        case C_GT:
        case C_LT:
            vm_code_gen_comparisons(fd, parser->cmd);
            break;
        case C_PUSH:
            vm_code_gen_push(fd, parser->segment, parser->arg2);
            break;
        case C_POP:
            vm_code_gen_pop(fd, parser->segment, parser->arg2);
            break;
        case C_FUNCTION:
            vm_code_gen_function(fd, &parser->arg1, parser->arg2);
            break;
        case C_LABEL:
            vm_code_gen_label(fd, &parser->arg1);
            break;
        case C_GOTO:
        case C_IF_GOTO:
            vm_code_gen_branching(fd, parser->cmd, &parser->arg1);
            break;
        case C_RETURN:
            vm_code_gen_return(fd);
            break;
        case C_CALL:
            vm_code_gen_call(fd, &parser->arg1, parser->arg2);
            break;
        case C_UNKNOWN:
            jackc_assert(false && "Unknown command found while generating code");
            break;
    }
}

void jackc_vm_code_gen_finalize(vm_code_generator* generator) {
    // Generate static variables
    if (generator->static_idx >= 0) {
        jackc_fprintf(generator->fd, "\n.data\n");

        for (uint32_t i = 0; i <= generator->static_idx; i++) {
            jackc_fprintf(generator->fd, "%s: .word 0\n", vm_code_gen_generate_static_name(i));
        }
    }

    // Add an empty line
    jackc_fprintf(generator->fd, "\n");
}
