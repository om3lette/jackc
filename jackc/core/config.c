#include "core/config.h"

jackc_config jackc_config_create(bool is_stack_growing_upwards, uint32_t upward_stack_size, bool code_comments) {
    return (jackc_config) {
        .is_stack_growing_upwards = is_stack_growing_upwards,
        .upward_stack_size = upward_stack_size,
        .code_comments = code_comments
    };
}
