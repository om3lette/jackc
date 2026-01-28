#ifndef JACKC_VM_TRANSLATOR_CONSTANTS_H
#define JACKC_VM_TRANSLATOR_CONSTANTS_H

/**
 * Maximum length of a function name.
 * Is contructed by a jackc compiler as <CLASS_NAME>.<FUNCTION/METHOD_NAME>
 */
#define MAX_FUNCTION_NAME_LENGTH 256

#define POINTER_THIS 0
#define POINTER_THAT 1


/**
 * Register used to store return value.
 */
#define RET_REG "a0"

/**
 * Register used to store base address of static variables.
 */
#define SEGMENT_STATIC_REG "gp"
#define STATIC_BASE_LABEL "STATIC_BASE"

/**
 * Source pointer register.
 */
#define JACK_SP_REG "sp"
#define SEGMENT_THIS_REG "s1"
#define SEGMENT_THAT_REG "s2"
#define SEGMENT_ARG_REG "s3"
#define SEGMENT_LCL_REG "fp"

/**
 * Register used to store the first argument of an operation.
 */
#define OP_ARG_1_REG "s7"

/**
 * Register used to store the second argument of an operation.
 */
#define OP_ARG_2_REG "s8"

/**
 * Register used to store the result of an operation.
 */
#define OP_RES_REG "s9"

/**
 * Temporary register used to load values from memory.
 */
#define LOAD_REG "s10"

#endif
