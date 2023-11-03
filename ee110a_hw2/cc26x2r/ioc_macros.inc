    .include "ioc_reg.inc"

IOCFG .MACRO pin, cfg
    MOV32   R1, cfg
    STR     R1, [R0, #(IOCFG_REGISTER_SIZE * pin)]
    .ENDM

IOCInit .MACRO
    MOV32   R0, IOC_BASE_ADDR
    .ENDM
