#ifndef JACKC_VM_TRANSLATOR_CONSTANTS_H
#define JACKC_VM_TRANSLATOR_CONSTANTS_H

#include "jackc_stdlib.h"

#define MAX_FUNCTION_NAME_LENGTH 128

#define POINTER_THIS 0
#define POINTER_THAT 1


#define RET_REG "a0"

#define JACK_SP_REG "sp"
#define SEGMENT_THIS_REG "s1"
#define SEGMENT_THAT_REG "s2"
#define SEGMENT_ARG_REG "s3"
#define SEGMENT_LCL_REG "fp"

#define OP_ARG_1_IDX 0
#define OP_ARG_1_REG ("t" TO_STRING(OP_ARG_1_IDX))

#define OP_ARG_2_IDX 1
#define OP_ARG_2_REG  ("t" TO_STRING(OP_ARG_2_IDX))

#define OP_RES_IDX 2
#define OP_RES_REG ("t" TO_STRING(OP_RES_IDX))

#define LOAD_IDX 3
#define LOAD_REG "t3"

#endif
