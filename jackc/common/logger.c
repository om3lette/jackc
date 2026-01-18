#include "logger.h"

bool jackc_should_log(jackc_log_level_t log_level) {
    return log_level >= LOG_LEVEL_DEBUG; // TODO: Implement logic to check if log level is enabled
}
