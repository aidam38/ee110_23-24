;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                         Keypad Interface Functions                         ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains the functions that interface with the 4x4 keypad used 
; in this demo. The functions defined are:
;   SelectRow - selects the row to be read
;   ReadRow - reads the selected row
; 
; Revision History: 10/27/23 Adam Krivka initial revision


; local include files
    .include "cc26x2r/gpio_reg.inc"
    .include "keypad_demo_symbols.inc"
    .include "macros.inc"

; export interface functions defined in this file
    .def SelectRow
    .def ReadRow


; SelectRow
;
; Description:      Selects the row to be read by setting the appropriate
;                   GPIO pins based on the input passed in R0.
;
; Inputs:           R0 - row to select
; Outputs:          None.
;
; Local Variables:  None.
; Shared Variables: None.
; Global Variables: None.
;
; Error Handling:   None.
;
; Registers Changed:flags, R0, R1
; Stack Depth:      1 word
;
; Algorithms:       None.
; Data Structures:  None.
;
; Revision History: 11/7/23  Adam Krivka      initial revision

SelectRow:
    PUSH {LR}                           ; save return address   

    AND    R0, #11b
    LSL    R0, #ROWSEL_A_PIN ; prepare row

    MOV32   R1, GPIO_BASE_ADDR
    STR     R0, [R1, #GPIO_DOUT_OFFSET] ;set pins

    POP {LR}                            ; restore return address
    BX LR

; ReadRow
;
; Description:          Reads the selected row and returns the value in R0 
;                       (as one-hot column encoding).
;
; Inputs:               None.
; Outputs:              R0 - one-hot column encoding of the selected row
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1
; Stack Depth:          1 word
;
; Algorithms:           None.
; Data Structures:      None.
;
; Revision History:     11/7/23  Adam Krivka      initial revision
ReadRow:
    PUSH    {LR}

    MOV32   R1, GPIO_BASE_ADDR
    LDR        R0, [R1, #GPIO_DIN_OFFSET]
    LSR     R0, #COLUMN_0_PIN

    POP     {LR}
    BX      LR
