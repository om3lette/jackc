#include "vm_code_generator.h"
#include "jackc_stdlib.h"

void jackc_vm_code_gen_free(vm_code_generator* generator) {
    jackc_free(generator);
}

void vm_code_gen_update_static_idx(vm_code_generator* generator, size_t idx) {
    generator->static_idx = MAX((long)idx, generator->static_idx);
}

vm_code_generator* jackc_vm_code_gen_init(const char* output_file, int fd, const jackc_config_t* config) {
    vm_code_generator* generator = jackc_alloc(sizeof(vm_code_generator));
    generator->output_file = output_file;
    generator->config = config;
    generator->fd = fd;
    generator->static_idx = -1;
    generator->label_idx = 0;
    generator->temp_idx = 0;
    generator->file_idx = 0;
    return generator;
}
