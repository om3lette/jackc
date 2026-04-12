Sys.init:
    call Memory.init
    call Screen.init
    call Output.init
    ret
Sys.halt:
    li a7, 93
    li a0, 0
    ecall
Sys.wait:
    li a7, 32
    lw a0, 0(s2)
    ecall
    ret

Keyboard.keyPressed:
    li   t6, 0xFFFF0000
    lw   t6, 0(t6)
    andi t6, t6, 1
    beqz t6, _JACKC_KEY_NOT_PRESSED
    li   t6, 0xFFFF0004
    lb   a0, 0(t6)
    ret
_JACKC_KEY_NOT_PRESSED:
    add  a0, x0, x0
    ret
