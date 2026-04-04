#include "vm_code_generator.h"
#include "std/jackc_stdlib.h"

void jackc_vm_code_gen_free(vm_code_generator* generator) {
    jackc_free(generator);
}

void vm_code_gen_update_static_idx(vm_code_generator* generator, size_t idx) {
    generator->static_idx = MAX((long)idx, generator->static_idx);
}

vm_code_generator jackc_vm_code_gen_init(const char* output_file, int fd, const jackc_config* config) {
    return (vm_code_generator) {
        .output_file = output_file,
        .config = config,
        .fd = fd,
        .n_local_args = 0,
        .label_idx = 0,
        .temp_idx = 0,
        .file_idx = 0,
    };
}
