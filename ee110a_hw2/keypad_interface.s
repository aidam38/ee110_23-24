    .include "cc26x2r/gpio_reg.inc"
    .include "macros.inc"
    .include "symbols.inc"

    .def SelectRow
    .def ReadRow

;reads from R0 and writes to the approriate select pins
SelectRow:
    PUSH {LR}

    AND    R0, #11b
    LSL    R0, #ROWSEL_A_PIN ; prepare row

    MOV32   R1, GPIO_BASE_ADDR
    STR     R0, [R1, #DOUT_OFFSET] ;set pins

    POP {LR}
    BX LR


ReadRow:
    PUSH    {LR}

    MOV32   R1, GPIO_BASE_ADDR
    LDR		R0, [R1, #DIN_OFFSET]
    LSR     R0, #COLUMN_0_PIN

    POP     {LR}
    BX      LR
