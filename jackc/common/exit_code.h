#ifndef EXIT_CODE_H
#define EXIT_CODE_H

typedef enum {
    JACKC_EXIT_SUCCESS = 0,
    JACKC_EXIT_MEMORY_ERROR = 1,
    JACKC_EXIT_INVALID_ARGUMENT = 2,
    JACKC_EXIT_FILE_ERROR = 3,
    JACKC_ASSERTION_ERROR = 4
} jackc_exit_code;

#endif
