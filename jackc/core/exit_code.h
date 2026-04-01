#ifndef EXIT_CODE_H
#define EXIT_CODE_H

typedef enum {
    JACKC_EXIT_ALLOCATION_ERROR = 1,
    JACKC_ASSERTION_ERROR = 2,
    JACKC_INVALID_NUMBER_OF_ARGUMENTS = 3
} jackc_exit_code;

#endif
