#ifndef EXIT_CODE_H
#define EXIT_CODE_H

typedef enum {
    JACKC_EXIT_SUCCESS = 0,
    JACKC_EXIT_MEMORY_ERROR,
    JACKC_EXIT_INVALID_ARGUMENT,
    JACKC_EXIT_FILE_ERROR,
} jackc_exit_code;

#endif
