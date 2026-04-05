#ifndef JACKC_CONFIG_H
#define JACKC_CONFIG_H

#include <stdint.h>

typedef struct {
    int is_stack_growing_upwards;
    uint32_t upward_stack_size;
} jackc_config;

jackc_config jackc_config_create(bool is_stack_growing_upwards, uint32_t upward_stack_size);

#endif
