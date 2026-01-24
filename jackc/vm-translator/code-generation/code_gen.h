#ifndef JACKC_VM_CODE_GENERATOR_H
#define JACKC_VM_CODE_GENERATOR_H

#include "vm-translator/parser.h"

typedef struct {
    char* output_file;
    int fd;
    size_t static_idx;
    size_t file_idx;
} vm_code_generator;

void vm_line_code_emit(const vm_code_generator* generator, const jackc_parser* parser);

// typedef enum {
//     C_UNKNOWN = 0,
//     C_ADD,
//     C_SUB,
//     C_NEG,
//     C_AND,
//     C_OR,
//     C_NOT,
//     C_EQ,
//     C_GT,
//     C_LT,typedef enum {
//     C_UNKNOWN = 0,
//     C_ADD,
//     C_SUB,
//     C_NEG,
//     C_AND,
//     C_OR,
//     C_NOT,
//     C_EQ,
//     C_GT,
//     C_LT,
//     C_PUSH,
//     C_POP,
//     C_LABEL,
//     C_GOTO,
//     C_IF_GOTO,
//     C_FUNCTION,
//     C_RETURN,
//     C_CALL
// } jackc_vm_cmd_type;


#endif
