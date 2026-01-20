#include "logger.h"
#include "jackc_stdlib.h"

bool jackc_should_log(jackc_log_level_t log_level) {
    return log_level >= LOG_LEVEL_DEBUG; // TODO: Implement logic to check if log level is enabled
}

void jackc_log(const char* fmt, jackc_log_level_t level, const char* caller, ...) {
    if (!jackc_should_log(level)) return;

    // TODO: Improve if strcat is introduced
    switch (level) {
        case LOG_LEVEL_DEBUG:
            jackc_printf("[DEBUG] ");
            break;
        case LOG_LEVEL_INFO:
            jackc_printf("[INFO] ");
            break;
        case LOG_LEVEL_WARNING:
            jackc_printf("[WARNING] ");
            break;
        case LOG_LEVEL_ERROR:
            jackc_printf("[ERROR] ");
            break;
        case LOG_LEVEL_FATAL:
            jackc_printf("[FATAL] ");
            break;
        default:
            jackc_printf("[UNKNOWN] ");
            break;
    }
    jackc_printf("%s: ", caller);

    va_list args;
    va_start(args, caller);
    jackc_vprintf(fmt, args);
    va_end(args);
}
