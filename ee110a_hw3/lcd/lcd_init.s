;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   lcd_init.s                               ;
;                           14-pin LCD Initialization                        ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file defines functions:
;   LCDInit
; 

; LCDInit
;
; Description:          Initializes the LCD by first initializing the GPT1 
;                       timers, and then sending the required set of commands
;                       to the LCD.  The commands are stored in a table in
;                       a table, and are sent to the LCD one at a time.
;
; Arguments:            r, c, str
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
; Registers Changed:    
; Stack Depth:          
; 
; Revision History:
;     

    .include "lcd_symbols.inc"
    .include "../macros.inc"

    .ref   LCDWaitForBusy
    .ref   LCDWriteNoTimer

    .def   LCDInit

    .text
    .align 4
LCDInitTab:
           ;Command         Delay Count
    .word   00111000b,     15000 * INITTIMER_COUNT_PER_US   ; wait for 150ms
    .word   00111000b,     4100 * INITTIMER_COUNT_PER_US    ; wait for 4.1ms
    .word   00111000b,     100 * INITTIMER_COUNT_PER_US     ; wait for 100us
    .word   00111000b,     -1      ; function set
    .word   00001000b,     -1      ; display off
    .word   00000001b,     -1      ; clear display
    .word   00000110b,     -1      ; entry mode set (increment cursor, no shift)
    .word   00001111b,     -1      ; display/cursor on

EndLCDInitTab:

; R4 = TIMER_BASE_ADDR
; R5 = tab pointer
LCDInit:
    PUSH    {LR, R4}        ; save return address and R4
    ; set up general timer
    MOV32   R4, TIMER_BASE_ADDR
    STREG   TIMER_CFG, R4, GPT_CFG_OFFSET

    ; set up command timer - timer A
    STREG   CMDTIMER_TAMR, R4, GPT_TAMR_OFFSET
    STREG   CMDTIMER_TAILR, R4, GPT_TAILR_OFFSET
    STREG   CMDTIMER_TAMATCHR, R4, GPT_TAMATCHR_OFFSET
    STREG   CMDTIMER_TAPR, R4, GPT_TAPR_OFFSET

    ; set up initialization timer - timer B
    STREG   INITTIMER_TBMR, R4, GPT_TBMR_OFFSET
    STREG   INITTIMER_TBPR, R4, GPT_TBPR_OFFSET

    ; get address of initialization table
    ADR     R5, LCDInitTab

LCDInitLoop:
    LDR     R1, [R5], #4    ; load command (DATA)
    LDR     R2, [R5], #4    ; load delay count

    ; check if delay count is -1
    CMP     R2, #-1
    BNE     LCDInitLoopWaitTimer ; if not use delay count
    ;B      LCDInitLoopWaitBusy  ; if yes wait for busy flag to clear

LCDInitLoopWaitBusy:
    BL      LCDWaitForBusy
    B       LCDInitLoopWrite

LCDInitLoopWaitTimer:
    STR     R2, [R4, #GPT_TBILR_OFFSET]        ; set timer A interval load register
    STREG   INITTIMER_ENABLE, R4, GPT_CTL_OFFSET; enable timer A

    ; wait until time out
LCDInitWaitTimeOut:
    LDR     R3, [R4, #GPT_RIS_OFFSET]; read raw interrupt status
    TST     R3, #GPT_RIS_TBTORIS     ; check if timer A timed out
    BEQ     LCDInitWaitTimeOut       ; if not, wait

    ;B      LCDInitLoopWrite

LCDInitLoopWrite:
    MOV32   R0, 0          ; want to write to register 0 during init
    ; R1 is already set up with DATA
    BL      LCDWriteNoTimer ; write to LCD
        
    ; if address is equal to EndLCDInitTab, break init loop
    ADR		R3, EndLCDInitTab
    CMP     R5, R3
    BNE     LCDInitLoop
    ;B      LCDInitEnd

LCDInitEnd:
    POP     {LR, R4, R5}        ; restore return address and R4
    BX      LR              ; return
