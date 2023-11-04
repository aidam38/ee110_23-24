    .include "cc26x2r/gpio_reg.inc"
    .include "macros.inc"
    .include "symbols.inc"

    .def SelectRow
    .def ReadRow

;reads from R0 and writes to the approriate select pins
SelectRow:
    PUSH {LR}



    POP {LR}
    BX LR


ReadRow:
    PUSH    {LR}

    MOV32   R1, GPIO_BASE_ADDR
    LDR		R0, [R1, #DIN_OFFSET]
    LSR     R0, #COLUMN_0_PIN

    POP     {LR}
    BX      LR
