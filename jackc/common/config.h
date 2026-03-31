#ifndef JACKC_CONFIG_H
#define JACKC_CONFIG_H

#include <stdint.h>

typedef struct {
    int is_stack_growing_upwards;
    uint32_t upward_stack_size;
} jackc_config;

jackc_config jackc_config_create(bool is_stack_growing_upwards, uint32_t upward_stack_size);

long config_word_to_bytes(const jackc_config* config, long word);

long config_stack_growth_coef(const jackc_config* config, long num);

long config_offset_by_idx(const jackc_config* config, int idx);

#endif
