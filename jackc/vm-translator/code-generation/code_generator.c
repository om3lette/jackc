#include "code_gen.h"
#include "common/jackc_assert.h"
#include "jackc_string.h"
#include "vm-translator/parser.h"

void vm_code_gen_arithmetic(int fd, jackc_vm_cmd_type cmd);

void vm_code_gen_push(int fd, const jackc_string* type, int arg2);
void vm_code_gen_pop(int fd, const jackc_string* type, int arg2);

void vm_code_gen_function(int fd, const jackc_string* name, int number_of_arguments);

void vm_code_gen_return(int fd);

void vm_code_gen_branching(int fd, jackc_vm_cmd_type cmd, const jackc_string* label);

void vm_code_gen_label(jackc_vm_cmd_type cmd, vm_code_generator* generator);

void vm_code_gen_call(int fd, jackc_vm_cmd_type cmd);

void vm_code_gen_comparisons(int fd, jackc_vm_cmd_type cmd);

void vm_code_gen_handle(vm_code_generator* generator, const jackc_parser* parser) {
    int fd = generator->fd;
    switch (parser->cmd) {
        case C_ADD:
        case C_SUB:
        case C_NEG:
        case C_AND:
        case C_OR:
        case C_NOT:
            vm_code_gen_arithmetic(fd, parser->cmd);
            break;
        case C_EQ:
        case C_GT:
        case C_LT:
            vm_code_gen_comparisons(fd, parser->cmd);
            break;
        case C_PUSH:
            vm_code_gen_push(fd, &parser->arg1, parser->arg2);
            break;
        case C_POP:
            vm_code_gen_pop(fd, &parser->arg1, parser->arg2);
            break;
        case C_FUNCTION:
            vm_code_gen_function(fd, &parser->arg1, parser->arg2);
            break;
        case C_LABEL:
            vm_code_gen_label(parser->cmd, generator);
            break;
        case C_GOTO:
        case C_IF_GOTO:
            vm_code_gen_branching(fd, parser->cmd, &parser->arg1);
            break;
        case C_RETURN:
            vm_code_gen_return(fd);
            break;
        case C_CALL:
            vm_code_gen_call(fd, parser->cmd);
            break;
        case C_UNKNOWN:
            jackc_assert(false && "Unknown command found while generating code");
            break;
    }
}
