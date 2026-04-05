#include "stack_frame.h"

int frame_offset_bytes(const frame_config* cfg, int word_idx) {
    return (cfg->grows_up ? -1 : 1) * cfg->word_size * word_idx;
}
