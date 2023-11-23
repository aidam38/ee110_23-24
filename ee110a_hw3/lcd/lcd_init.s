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
    .include "../cc26x2r/gpio_reg.inc"
	.include "../cc26x2r/ioc_reg.inc"
	.include "../cc26x2r/ioc_macros.inc"


    .ref   LCDWaitForNotBusy
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
    PUSH    {LR, R4, R5, R6, R7, R8}        ; save return address and R4

    ; set up IO pins
    IOCInit

    ; control pins
    IOCFG       RS_PIN, IOCFG_GENERIC_OUTPUT      ; RS pin
    IOCFG       RW_PIN, IOCFG_GENERIC_OUTPUT      ; RW pin
    IOCFG       E_PIN,IOCFG_GENERIC_OUTPUT        ; E pin

    ; enable output for control pins 
    MOV32       R1, GPIO_BASE_ADDR
    MOV32       R0, ((1 << RW_PIN) | (1 << RS_PIN) | (1 << E_PIN))
    STR         R0, [R1, #GPIO_DOE_OFFSET]

    ; (data pins are configure before each read/write call)

    ; set up timer
    MOV32   R4, TIMER_BASE_ADDR
    STREG   TIMER_CFG, R4, GPT_CFG_OFFSET

    ; set up command timer - timer A
    STREG   CMDTIMER_TAMR, R4, GPT_TAMR_OFFSET
    STREG   CMDTIMER_TAILR, R4, GPT_TAILR_OFFSET
    STREG   CMDTIMER_TAMATCHR, R4, GPT_TAMATCHR_OFFSET
    STREG   CMDTIMER_TAPR, R4, GPT_TAPR_OFFSET

    ; start it so that it's timed out for first read
    STREG   CMDTIMER_ENABLE, R4, GPT_CTL_OFFSET

    ; start init loop
    ; get address of initialization table
    ADR     R5, LCDInitTab
    ADR		R6, EndLCDInitTab
LCDInitLoop:
    LDR     R7, [R5], #4    ; load command (DATA)
    LDR     R8, [R5], #4    ; load delay count

    ; check if delay count is -1
    CMP     R8, #-1
    BNE     LCDInitLoopWaitTimer ; if not use delay count
    ;B      LCDInitLoopWaitBusy  ; if yes wait for busy flag to clear

LCDInitLoopWaitBusy:
    BL      LCDWaitForNotBusy
    B       LCDInitLoopWrite

; use noops
LCDInitLoopWaitTimer:
    SUBS	R8, #1
    BNE		LCDInitLoopWaitTimer
    ;B      LCDInitLoopWrite

LCDInitLoopWrite:
    ; prepare arguments
    MOV32   R0, 0          ; want to write to register 0 during init
    MOV		R1, R7
    BL      LCDWriteNoTimer ; write to LCD
        
    ; if address is equal to EndLCDInitTab, break init loop
    CMP     R5, R6
    BNE		LCDInitLoop

LCDInitEnd:
    POP     {LR, R4, R5, R6, R7, R8}        ; restore return address and R4
    BX      LR              ; return
