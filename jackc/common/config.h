#ifndef JACKC_CONFIG_H
#define JACKC_CONFIG_H

#include <stdint.h>

typedef struct {
    int is_stack_growing_upwards;
    uint32_t upward_stack_size;
} jackc_config_t;

const jackc_config_t* jackc_config_create(bool is_stack_growing_upwards, uint32_t upward_stack_size);

long config_word_to_bytes(const jackc_config_t* config, long word);

long config_stack_growth_coef(const jackc_config_t* config, long num);

long config_offset_by_idx(const jackc_config_t* config, int idx);

#endif
