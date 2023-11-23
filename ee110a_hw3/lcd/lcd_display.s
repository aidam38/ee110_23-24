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



; local includes
    .include "../std.inc"
    .include "lcd_symbols.inc"

; import functions from other files
    .ref LCDWrite
    .ref LCDWaitForNotBusy

; export functions to other files
    .def Display
    .def DisplayChar



; SetCursorPos
;
; Description:          Sets cursor position on the LCD. 
;
; Arguments:            r in R0, c in R1
; Return Values:        success/fail in R0.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       If incorrect row and column values are passed, 
;                       returns FUNCTION_FAIL
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          0
; 
; Revision History:
;     

CursorBaseAddr:
    .byte ROW_0_START, ROW_1_START, ROW_2_START, ROW_3_START

SetCursorPos:
    PUSH    {LR, R4, R5}                ; save return address and used registers

; check that row and column are in bounds
    CMP     R0, #0              ; check if r >= 0
    BLT     SetCursorPosFail    ; if r < 0, fail

    CMP     R0, #NUM_ROWS       ; check if r < NUM_ROWS
    BGE     SetCursorPosFail    ; if r >= NUM_ROWS, fail

    CMP     R1, #0              ; check if c >= 0
    BLT     SetCursorPosFail    ; if c < 0, fail

    CMP     R1, #NUM_COLS       ; check if c < NUM_COLS
    BGE     SetCursorPosFail    ; if c >= NUM_COLS
    ;B      SetCursorPosValid

SetCursorPosValid:
    MOVA    R4, CursorBaseAddr   ; load address of cursor base addresses
    LDRB    R5, [R4, R0]         ; load CursorBaseAddr[r] to R5
    ADD     R5, R1               ; add c to address
    ;B      SetCursorPosWrite

SetCursorPosWrite:
    BL      LCDWaitForNotBusy   ; wait for LCD to be ready

    ORR     R5, #SET_DDRAM_ADDR ; prepare a set DDRAM command
    MOV     R0, R5
    MOV32   R1, 0               ; RS = 0
    BL      LCDWrite            ; write set DDRAM command to LCD

    B       SetCursorPosSuccess

SetCursorPosFail:
    MOV32   R0, FUNCTION_FAIL   ; prepare fail return value
    B       SetCursorPosDone

SetCursorPosSuccess:
    MOV32   R0, FUNCTION_SUCCESS; prepare success return value
    ;B      SetCursorPosDone

SetCursorPosDone:
    POP     {LR, R4, R5}        ; restore return address and used registers
    BX      LR                  ; return


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
; Error Handling:       If if the string ever goes out of screen,
;                       the function returns a fail value
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          0
; 
; Revision History:
;     

Display:
    PUSH    {LR, R4, R5}            ; save return address and used registers

    MOV     R4, R1                  ; save column
    MOV     R5, R2                  ; save string pointer

    BL      SetCursorPos            ; set cursor position based on r, c

    CMP     R0, FUNCTION_FAIL       ; check if setting cursor failed
    BEQ     DisplayFail             ; fail this function too if so

DisplayLoop:
    LDRB    R5, [R4], #1            ; load character from str (post-incr address)
    ADD     R1, #1                  ; add 1 to column
    
    CMP     R1, #NUM_COLS           ; check if we're off screen
    BGT     DisplayFail             ; if yes, stop and return fail value

    CMP     R5, #0                  ; check if character is null terminator
    BEQ     DisplaySuccess          ; if yes, we're done printing the string

    BL      LCDWaitForNotBusy       ; wait for LCD to be ready

    ; prepare arguments for writing to LCD
    MOV32   R0, 1                   ; RS = 1
    MOV     R1, R5                  ; copy character
    BL      LCDWrite                ; write data to LCD

    B       DisplayLoop             ; loop

DisplayFail:
    MOV32   R0, FUNCTION_FAIL       ; prepare fail return value
    B       DisplayDone

DisplaySuccess:
    MOV32   R0, FUNCTION_SUCCESS    ; prepare success return value
    ;B      DisplayDone

DisplayDone:
    POP     {LR, R4, R5}        ; save return address
    BX      LR                  ; return



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
; Error Handling:       If the row and column are out of bounds, the function
;                       returns a fail value.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          0
; 
; Revision History:
;     

DisplayChar:
    PUSH    {LR, R4, R5}                ; save return address and used registers

    MOV     R4, R1              ; save column
    MOV     R5, R2              ; save character

    BL      SetCursorPos        ; set cursor position based on r, c

    CMP     R0, FUNCTION_FAIL   ; check if setting cursor failed
    BEQ     DisplayCharFail         ; fail this function too if so

    ; write character to LCD
    MOV32   R0, 1               ; RS = 1
    MOV     R1, R5              ; copy character
    BL      LCDWrite            ; write data to LCD

    B       DisplayCharSuccess  ; we've successfully written the character

DisplayCharFail:
    MOV32   R0, FUNCTION_FAIL   ; prepare fail return value
    B       DisplayCharDone

DisplayCharSuccess:
    MOV32   R0, FUNCTION_SUCCESS; prepare success return value
    ;B      DisplayCharDone

DisplayCharDone:
    POP     {LR, R4, R5}        ; save return address
    BX      LR                  ; return




; ClearDisplay
;
; Description:          Clears the LCD display
;
; Arguments:            None
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       If the row and column are out of bounds, the function
;                       returns a fail value.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          0
; 
; Revision History:
;     

ClearDisplay:
    PUSH    {LR}

    BL      LCDWaitForNotBusy   ; wait for LCD to not be busy

    ; call LCDWrite(CLEAR_DISPLAY, RS = 1)
    MOV32   R0, CLEAR_DISPLAY
    MOV32   R1, 1
    BL      LCDWrite

    POP     {LR}
    BX      LR