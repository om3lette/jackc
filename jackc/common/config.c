#include "common/config.h"
#include "jackc_stdlib.h"

const jackc_config_t* jackc_config_create(bool is_stack_growing_upwards, uint32_t upward_stack_size) {
    jackc_config_t* config = jackc_alloc(sizeof(jackc_config_t));
    config->is_stack_growing_upwards = is_stack_growing_upwards;
    config->upward_stack_size = upward_stack_size;
    return config;
}

long config_stack_growth_coef(const jackc_config_t* config, long num) {
    return config->is_stack_growing_upwards ? num : -num;
}

long config_word_to_bytes(const jackc_config_t* config, long word) {
    // potential 64bit mode support
    (void) config;
    return word * 4;
}

long config_offset_by_idx(const jackc_config_t* config, int idx) {
    return config_stack_growth_coef(config, config_word_to_bytes(config, idx));
}
