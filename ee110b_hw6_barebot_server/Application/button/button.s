;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   keypad.s                                 ;
;                                4x4 Button Drive                            ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the code for the keypad driver. It is responsible for
; scanning the keypad and debouncing the keys. It is also responsible for
; generating events when keys are pressed and released.
;
; The functions implemented in this file are:
;   ButtonInit - initializes the keypad driver
;   ButtonRegisterHwi - registers direct hardware interrupt for the keypad
;                       (for use in assembly-only code)
;   ButtonScanAndDebounce - scans the keypad and debounces the keys
;
; The interface, expected to be implemented in a separate file for flexibility,
; is as follows:
;  KeyPressed - gets called with {uint8_t row, uint8_t column} when key pressed
; 
; Revision History:
;     11/7/23  Adam Krivka      initial revision
;     3/4/24    Adam Krivka     expanded ButtonInit and created KeypadRegisterHwi
;     3/6/24    Adam Krivka     added comments and fixed formatting


; local include files
    .include "button_symbols.inc"
    .include "../std.inc"

; import functions defined in other files
    .ref ButtonPressed

; export functions defined in this file
    .def ButtonInit
    .def ButtonDebounce
    .def ButtonRTOSHwiHandler


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MEMORY
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .data
    .align 8

; DebounceCounter - the current debounce counter (debounce time is usually
;                    less than 8 bits)
DebounceCounter: .byte 0

; PrevState - the previous state of one row of the keypad (one-hot encoding)
PrevState: .byte 0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .text

; KeypadInit
;
; Description:          Initializes the keypad driver by setting the current row to 0,
;                       the debounce counter to DEBOUNCE_TIME, and the previous state
;                       to 1111b (all keys up).
;
; Arguments:            None.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Inputs:               None.
; Outputs:              None.
;
; Error Handling:       None.
;
; Registers Changed:    R0, R1
; Stack Depth:          1
; 
; Revision History:
;       11/7/23     Adam Krivka      initial revision
;       3/4/24      Adam Krivka      included GPIO init and timer init functionality
;       3/6/24      Adam Krivka      added comments and fixed formatting

ButtonInit:
    PUSH    {LR}            ; save return address


; initialize variables
    ; initialize DebounceCounter to DEBOUNCE_TIME
    MOVA    R0, DebounceCounter
    MOV     R1, #DEBOUNCE_TIME
    STRB    R1, [R0]

    ; initialize PrevState to 1111b
    MOVA    R0, PrevState
    MOV     R1, #BOTH_BUTTONS_UP
    STRB    R1, [R0]

; configure GPIO pins for keypad
    MOV32   R1, IOC_BASE_ADDR   ; prepare IOC base address
    STREG   BUTTON_CFG, R1, IOCFG_REG_SIZE * BUTTON_0_PIN
    STREG   BUTTON_CFG, R1, IOCFG_REG_SIZE * BUTTON_1_PIN


 ; configure timer
    MOV32   R1, TIMER_BASE_ADDR
    STREG   TIMER_CFG, R1, GPT_CFG_OFFSET
    STREG   TIMER_IMR, R1, GPT_IMR_OFFSET
    STREG   TIMER_TAMR, R1, GPT_TAMR_OFFSET
    STREG   TIMER_TAILR, R1, GPT_TAILR_OFFSET
    STREG   TIMER_TAPR, R1, GPT_TAPR_OFFSET
    STREG   TIMER_CTL, R1, GPT_CTL_OFFSET ; starts timer

    POP     {LR}            ; restore return address
    BX      LR






; ButtonRTOSHwiHandler
;
; Description:       This is the event handler for the RTOS-created hardware 
;                    interrupt, which is used to post the software interrupt
;                    so that we're scanning and debouncing in lower priority task.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  timeout interrupt clear bit
; Global Variables:  None.
;
; Input:             None.
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
; Registers Changed: None
; Stack Depth:       1 word
;
; Revision History:
;     3/6/24  Adam Krivka      initial revision

; pull necessary symbols 
    .ref    swiTask
    .ref    ti_sysbios_knl_Swi_post
ButtonRTOSHwiHandler:
    PUSH    {LR}                ; save LR

; clear the timer time-out interrupt
    MOV32   R1, TIMER_BASE_ADDR
    STREG   GPT_ICLR_TATOCINT_CLEAR, R1, GPT_ICLR_OFFSET

; Swi_post(swiTask)
    MOVA    R1, swiTask
    LDR     R0, [R1]
    BL      ti_sysbios_knl_Swi_post

    POP     {LR}                ; restore LR
    BX      LR                  ; return



; ButtonScanAndDebounce
;
; Description:      Scans the keypad and debounces the keys. If a key is pressed,
;                   for DEBOUNCE_TIME, the KeyPressed function is call with
;                         [ROW][COLUMN]
;                   where
;                        ROW is the row of the key pressed (8-bit binary)
;                        COLUMN is the column of the key pressed (8-bit binary)
;
; Operation:        This function is called periodically by the timer. Each
;                   time it is called, it checks whether the debounce counter
;                   is not its maximum value (DEBOUNCE_TIME), which indicates
;                   that a key is being debounced, in which case it checks
;                   whether that key is still being pressed, and if yes decrements
;                   the debounce counter further, and if not resets it to 
;                   its maximum value. If the debounce counter is zero, it enqueus
;                   an event as described above. If no key is pressed, the curent
;                   row is incremented and the function returns.
;
; Arguments:        None.
; Return Values:    None.
;
; Local Variables:     
;         R4 = CurrentRow address
;         R5 = DebounceCounter address
;         R6 = PrevState addresss
;         R7 = CurrentRow value
;         R8 = DebounceCounter value
; Shared Variables: None.
; Global Variables: None.
;
; Inputs:           Button.
; Outputs:          Calling KeyPressed function.
;
; Error Handling:   None.
;
; Registers Changed: flags, R0, R1
; Stack Depth:        6
;
; Revision History:
;     11/7/23  Adam Krivka      initial revision
;     3/6/24   Adam Krivka      change from EnqueueEvent to KeyPressed


ButtonDebounce:
    PUSH    {LR, R4, R5, R6, R7, R8}        ; save return address and registers

    ; load DebounceCounter address and value
    MOVA    R5, DebounceCounter
    LDRB    R8, [R5]

Read:
    ; read the current state of the keypad
    MOV32   R1, GPIO_BASE_ADDR
    LDR     R0, [R1, #GPIO_DIN_OFFSET]
    LSR     R0, #BUTTON_0_PIN       ; shift to the right to get the column bits
    AND     R0, #BOTH_BUTTONS_UP              ; mask off the upper bits
    ;B      Compare               ; compare CurState to PrevState

Compare:
    ; load PrevState address and value
    MOVA    R6, PrevState
    LDRB    R1, [R6]

    ; if PrevState != CurState, reset debounce state
    CMP     R0, R1
    BNE     ResetDebounce

    ; if PrevState == CurState AND CurState == 1111, no key is pressed
    ; nor was it pressed last time, so we can just end the function
    CMP     R0, #BOTH_BUTTONS_UP
    BEQ     ButtonScanAndDebounceEnd

    ; if PrevState == CurState AND CurState != 1111 (meaning key is continuously
    ; being pressed), debounce
    ;B      Debounce



; Debounce current keypress using DebounceCounter
Debounce:
    SUBS    R8, #1               ; decrement DebounceCounter value

    ; if DebounceCounter < 0
    BMI     DebounceCounterNegative 

    ; if DebounceCounter > 0
    BNE     DebounceEnd 

    ; if DebounceCounter == 0
    ;B      DebounceCounterZero 

DebounceCounterZero:
    BL      ButtonPressed
    B       DebounceEnd

DebounceCounterNegative:
    ; We need to reset DebounceCounter to zero so we don't overflow in the negatives
    MOV     R8, #0 
    ;B      DebounceEnd

DebounceEnd:
    ; store new DebounceCounter value (always done regardless of DebounceCounter
    ; value)
    STRB    R8, [R5] 
    B       ButtonScanAndDebounceEnd        ; end function

; Reset DebounceCounter
ResetDebounce:
    STRB    R0, [R6]                        ; update PrevState, because in this 
                                            ; clause we know that PrevState != CurState

    ; Reset Debounce Counter
    MOV     R8, #DEBOUNCE_TIME

    ; If a key is pressed, decrement DebounceCounter to start 
    ; debouncing in the next call
    CMP     R0, #BOTH_BUTTONS_UP
    BEQ     ResetDebounceEnd
    ;B        KeyStartPressed

KeyStartPressed:
    SUB     R8, #1          ; decrement DebounceCounter so we start debouncing next time

ResetDebounceEnd:
    STRB    R8, [R5]                        ; store new DebounceCounter value in memory
    ;B        ButtonScanAndDebounceEnd      ; end function

ButtonScanAndDebounceEnd:
    POP     {LR, R4, R5, R6, R7, R8}        ; restore return address and registers
    BX      LR
