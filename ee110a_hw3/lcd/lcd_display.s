;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                 lcd_display.s                              ;
;                               4x4 Keypad Driver                            ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file defines functions:
;   Display - display a string
;   DisplayChar - display a single character
; 

    .include "../constants.inc"
    .include "../macros.inc"
    .include "lcd_display.inc"

    .ref LCDWrite
    .ref LCDWaitForBusy

    .def    Display
    .def    DisplayChar

; Display
;
; Description:         Displays a string starting at the given row and column.
;                      Uses whatever cursor incrementing mode is currently
;                      set on the LCD. 
;
; Arguments:            r in R0, c in R1, str in R2
; Return Values:        success/fail.
;
; Local Variables:      r, c, character in R3
; Shared Variables:     None.
; Global Variables:     None.
;
; Inputs:               Function arguments.
; Outputs:              LCD.
;
; Error Handling:       If if the string ever goes out of screen,
;                       the function returns a fail value
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          0
; 
; Revision History:
;     

Display:
    PUSH    {LR}        ; save return address

    ; set DD RAM command
    MOV32   R0, #SET_DDRAM_ADDR
    MOV32   R1, #0      ; RS = 0
    BL      LCDWrite

DisplayLoop:
    ; load character from str (post-increment address)
    LDRB    R1, [R2], #1

    ; check if character is null terminator
    CMP     R1, #0
    BEQ     DisplayDone ; if yes, we're done

    BL      LCDWaitForBusy  ; wait for LCD to be ready

    ; prepare arguments for writing to LCD
    MOV32   R0, #1      ; RS = 1
    ; character is already in R1, LCDWaitForBusy should mangle it 
    BL      LCDWrite        ; write data to LCD

    B       DisplayLoop ; loop

DisplayDone:
    ; read cursor and if it's out of bounds, return FUNCTION_FAIL
    ; TODO

    ; otherwise return FUNCTION_SUCCESS
    MOV     R0, FUNCTION_SUCCESS
    POP     {LR}        ; save return address
    BX      LR          ; return



; DisplayChar
;
; Description:          Displays a single character at the given row and column.
;
; Arguments:            r, c, ch
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Inputs:               Function arguments.
; Outputs:              LCD.
;
; Error Handling:       If the row and column are out of bounds, the function
;                       returns a fail value.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          0
; 
; Revision History:
;     

DisplayChar:
    PUSH    {LR}        ; save return address

    ; set DD RAM command
    MOV32   R0, #SET_DDRAM_ADDR
    MOV32   R1, #0      ; RS = 0
    BL      LCDWrite

    ; write character to LCD
    MOV32   R0, #1      ; RS = 1
    MOV     R1, R2      ; character is originally in R2
    BL      LCDWrite

    ; read cursor and if it's out of bounds, return FUNCTION_FAIL
    ; TODO

    ; otherwise return FUNCTION_SUCCESS
    MOV     R0, FUNCTION_SUCCESS
    POP     {LR}        ; save return address
    BX      LR          ; return

