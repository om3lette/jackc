#ifndef JACKC_LOGGER_H
#define JACKC_LOGGER_H

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR
} jackc_log_level_t;

bool jackc_should_log(jackc_log_level_t log_level);

// TODO: Improve if strcat is introduced
#define LOG_WITH_CALLER(msg, level, caller) do { \
    if (jackc_should_log(LOG_LEVEL_DEBUG)) { \
        jackc_print_string("[TODO] "); \
        jackc_print_string(caller);\
        jackc_print_string(": ");\
        jackc_print_string(msg); \
        jackc_print_newline(); \
    } \
} while (0)

#define LOG_DEBUG(msg) LOG_WITH_CALLER(msg, LOG_LEVEL_DEBUG, __func__)

#define LOG_INFO(msg) LOG_WITH_CALLER(msg, LOG_LEVEL_INFO, __func__)

#define LOG_ERROR(msg) LOG_WITH_CALLER(msg, LOG_LEVEL_ERROR, __func__)

#endif
