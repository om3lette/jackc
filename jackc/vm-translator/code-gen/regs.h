#ifndef JACKC_ASM_CODE_GEN_H
#define JACKC_ASM_CODE_GEN_H

#define REG_ZERO     "x0"
#define REG_RET_ADDR "ra"
#define REG_SP       "sp"
#define REG_LOCAL    "s1"
#define REG_ARG      "s2"
#define REG_THIS     "s3"
#define REG_THAT     "s4"
#define REG_TEMP     "s5"
#define REG_STATIC   "gp"
#define REG_SCRATCH  "t0"   // for loads before stores
#define REG_OP1      "t1"   // left operand
#define REG_OP2      "t2"   // right operand
#define REG_RES      "t3"   // operation result
#define REG_RET      "a0"

#define POINTER_THIS 0
#define POINTER_THAT 1

#endif
