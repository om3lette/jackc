#ifndef JACKC_VM_CODE_GENERATOR_H
#define JACKC_VM_CODE_GENERATOR_H

#include "common/config.h"
#include "vm-translator/parser/vm_parser.h"

typedef struct {
    const char* output_file;
    const jackc_config_t* config;
    int fd;
    long static_idx;
    size_t label_idx;
    size_t temp_idx;
    size_t file_idx;
} vm_code_generator;

vm_code_generator* jackc_vm_code_gen_init(const char* output_file, int fd, const jackc_config_t* config);

void jackc_vm_code_bootstrap(vm_code_generator* generator);

void jackc_vm_code_gen_line(vm_code_generator* generator, const jackc_parser* parser);

void jackc_vm_code_gen_finalize(vm_code_generator* generator);

void jackc_vm_code_gen_free(vm_code_generator* generator);

void vm_code_gen_update_static_idx(vm_code_generator* generator, size_t idx);

#endif
