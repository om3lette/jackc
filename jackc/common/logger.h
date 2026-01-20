#ifndef JACKC_LOGGER_H
#define JACKC_LOGGER_H

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} jackc_log_level_t;

bool jackc_should_log(jackc_log_level_t log_level);

void jackc_log(const char* fmt, jackc_log_level_t log_level, const char* msg, ...);

#define LOG_DEBUG(msg, ...) jackc_log(msg, LOG_LEVEL_DEBUG, __func__, ##__VA_ARGS__)

#define LOG_INFO(msg, ...) jackc_log(msg LOG_LEVEL_INFO, __func__, ##__VA_ARGS__)

#define LOG_WARNING(msg, ...) jackc_log(msg, LOG_LEVEL_WARNING, __func__, ##__VA_ARGS__)

#define LOG_ERROR(msg, ...) jackc_log(msg, LOG_LEVEL_ERROR, __func__, ##__VA_ARGS__)

#define LOG_FATAL(msg, ...) jackc_log(msg, LOG_LEVEL_FATAL, __func__, ##__VA_ARGS__)

#endif
