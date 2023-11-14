;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                    lcd_io.s                                ;
;                                 14-pin LCD I/O                             ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file defines functions:
;   LCDWrite
;   LCDWriteNoTimer
;   LCDRead
;   LCDWaitForBusy
; 

	.include "../cc26x2r/gpio_reg.inc"
	.include "../cc26x2r/gpt_reg.inc"
	.include "lcd_demo_symbols.inc"
    .include "lcd_symbols.inc"
    .include "../macros.inc"

    .def LCDWrite
    .def LCDWriteNoTimer
    .def LCDRead
    .def LCDWaitForBusy
    
; LCDWrite
;
; Description:          Writes an 8-bit value to the LCD, to the register
;                       selected by the RS argument. This function waits for
;                       the LCD to finish processing the previous command, 
;                       and starts a 1ms cycle timer so that the next invocation
;                       of this function will not be called before 1ms has
;                       passed.
;
; Arguments:            None.
; Return Values:        data in R0, RS in R1
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

; R4 = GPIO_BASE_ADDR
; R5 = TIMER_BASE_ADDR
; R2-R3 = scratch
LCDWrite:
    PUSH    {LR, R4, R5}                ; save LR, R4, R5

    ; load base addresses of 
    ;  - GPIO (R4)
    ;  - timer (R5)
    MOV32   R4, GPIO_BASE_ADDR          ; prepare to manipulate GPIO pins
    MOV32   R5, TIMER_BASE_ADDR         ; prepare to manipulate timer

    ; wait for command timer to be timed-out
LCDWriteWaitTimeOut:
    LDR     R2, [R5, #GPT_RIS_OFFSET]; read raw interrupt status
    TST     R2, #GPT_RIS_TATORIS     ; check if timer A timed out
    BNE     LCDWriteWaitTimeOut              ; if not, wait

    ; clear interrupt
    BIC     R2, #GPT_RIS_TATORIS      ; clear timer A time out interrupt
    STR     R2, [R5, #GPT_ICRL_OFFSET]; write to interrupt clear register

    ; write RS based on argument and R/W low
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; load DOUT register
    ORR     R2, R0, LSL #RS_PIN         ; merge RS into DOUT while
                                        ; shifting to the RS bit position
    BIC     R2, #(1 << RW_PIN)         ; merge R/W = 0 into DOUT while
                                        ; shifting to the R/W bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; wait for 140ns (round_up(140 / 40ns) = 3.5 ~ 4)
    MOV     R2, #4
LCDWriteWaitNoTimer:
    SUBS    R2, #1      ; decrement counter
    BNE     LCDWriteWaitNoTimer

    ; write E high and data
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R2, #(1 << E_PIN)          ; merge E = 1 into DOUT while
                                        ; shifting to the E bit position
    ORR     R2, R1, LSL #DATA_0_PIN     ; merge data into DOUT while
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; start command timer
    STREG   CMDTIMER_ENABLE, R5, GPT_CTL_OFFSET

    ; wait for 450ns by checking the match interrupt
LCDWriteWaitMatch:
    LDR     R2, [R5, #GPT_RIS_OFFSET]; read raw interrupt status
    TST     R2, #GPT_RIS_TAMRIS      ; check if timer A reached match
    BNE     LCDWriteWaitMatch                ; if not, wait

    ; clear interrupt
    BIC     R2, #GPT_RIS_TAMRIS       ; clear timer A match interrupt
    STR     R2, [R5, #GPT_ICRL_OFFSET]; write to interrupt clear register

    ; write E low
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R2, #(1 << E_PIN)           ; merge E = 0 into DOUT while
                                        ; shifting to the E bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    POP     {LR, R4, R5}                ; restore LR, R4, R5
    BX      LR                          ; return

; LCDWriteNoTimer
;
; Description:          Writes an 8-bit value to the LCD, to the register
;                       selected by the RS argument. This function does not
;                       use any timers and instead relies on noop loops to
;                       generate the necessary delays. This function is intended
;                       to be used in the initialization routine, where the LCD
;                       timer is used for a different purpose (generate delays
;                       between sending commands). For writing to the LCD after
;                       initialization, use LCDWrite!
;
; Arguments:            RS in R0, DATA in R1
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

; R4 = GPIO_BASE_ADDR
; R2-R3 = scratch
LCDWriteNoTimer:
    PUSH    {LR, R4}                ; save LR, R4

    ; load base addresses of 
    ;  - GPIO (R4)
    MOV32   R4, GPIO_BASE_ADDR          ; prepare to manipulate GPIO pins


    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; load DOUT register
    ORR     R2, R0, LSL #RS_PIN         ; merge RS into DOUT while
                                        ; shifting to the RS bit position
    BIC     R2, #(1 << RW_PIN)         ; merge R/W = 0 into DOUT while
                                        ; shifting to the R/W bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; wait for 140ns (round_up(140 / 40ns) = 3.5 ~ 4)
    MOV     R3, #4
LCDWriteNoTimerWait1:

    SUBS    R3, #1      ; decrement counter
    BNE     LCDWriteNoTimerWait1

    ; write E high and data
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R2, #(1 << E_PIN)          ; merge E into DOUT while
                                        ; shifting to the E bit position
    ORR     R2, R1, LSL #DATA_0_PIN     ; merge data into DOUT while
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; wait for 450ns (round_up(450 / 40ns) = 11.25 ~ 12)
    MOV     R3, #12
LCDWriteNoTimerWait2:
    SUBS    R3, #1      ; decrement counter
    BNE     LCDWriteNoTimerWait2

    ; write E low
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R2, #(1 << E_PIN)           ; merge E = 0 into DOUT while
                                        ; shifting to the E bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    POP     {LR, R4}              ; restore LR, R4
    BX      LR                          ; return


; LCDRead
;
; Description:          Reads an 8-bit value from the LCD, from the register
;                       selected by the RS argument. This function waits for
;                       the LCD to finish processing the previous command,
;                       and starts a 1ms cycle timer so that the next invocation
;                       of this function will not be called before 1ms has
;                       passed.
;
; Arguments:            RS in R0
; Return Values:        data in R0
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

; R4 = GPIO_BASE_ADDR
; R5 = TIMER_BASE_ADDR
; R2-R3 = scratch
LCDRead:
    PUSH    {LR, R4, R5}                ; save LR, R4, R5

    ; load base addresses of 
    ;  - GPIO (R4)
    ;  - timer (R5)
    MOV32   R4, GPIO_BASE_ADDR          ; prepare to manipulate GPIO pins
    MOV32   R5, TIMER_BASE_ADDR         ; prepare to manipulate timer

    ; wait for command timer to be timed-out
LCDReadWaitTimeOut:
    LDR     R2, [R5, #GPT_RIS_OFFSET]; read raw interrupt status
    TST     R2, #GPT_RIS_TATORIS     ; check if timer A timed out
    BEQ     LCDReadWaitTimeOut              ; if not, wait

    ; clear interrupt
    BIC     R2, #GPT_RIS_TATORIS      ; clear timer A time out interrupt
    STR     R2, [R5, #GPT_ICRL_OFFSET]; write to interrupt clear register

    ; write RS based on argument and R/W low
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; load DOUT register
    ORR     R2, R1, LSL #RS_PIN         ; merge RS into DOUT while
                                        ; shifting to the RS bit position
    BIC     R2, #(1 << RW_PIN)          ; merge R/W = 0 into DOUT while
                                        ; shifting to the R/W bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; wait for 140ns (round_up(140 / 40ns) = 3.5 ~ 4)
    MOV     R2, #4
LCDReadWaitNoTimer:
    SUBS    R2, #1      ; decrement counter
    BNE     LCDReadWaitNoTimer

    ; write E high and data
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R2, #(1 << E_PIN)           ; merge E = 1 into DOUT while
                                        ; shifting to the E bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; start command timer
    STREG   CMDTIMER_ENABLE, R5, GPT_CTL_OFFSET

    ; wait for 450ns by checking the match interrupt
LCDReadWaitMatch:
    LDR     R2, [R5, #GPT_RIS_OFFSET]; read raw interrupt status
    TST     R2, #GPT_RIS_TAMRIS      ; check if timer A reached match
    BEQ     LCDReadWaitMatch                ; if not, wait

    ; clear interrupt
    BIC     R2, #GPT_RIS_TAMRIS       ; clear timer A match interrupt
    STR     R2, [R5, #GPT_ICRL_OFFSET]; write to interrupt clear register

    ; write E low
    LDR     R0, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R0, #(1 << E_PIN)           ; merge E = 0 into DOUT while
                                        ; shifting to the E bit position
    STR     R0, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; recover data
    LSR     R0, #DATA_0_PIN             ; shift data to the right
    AND     R0, #0xFF                   ; mask out the rest of the bits

    POP     {LR, R4, R5}                ; restore LR, R4, R5
    BX      LR                          ; return

; LCDWaitForBusy
;
; Description:          Reads the busy flag of the LCD and waits until it is
;                       cleared.
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
; Registers Changed:    
; Stack Depth:          
; 
; Revision History:
;     

LCDWaitForBusy:
    PUSH    {LR}
LCDWaitForBusyLoop:
    ; read busy flag
    MOV     R0, #0
    BL      LCDRead

    ; test if busy flag is set
    TST     R0, #BUSY_FLAG_MASK
    BEQ     LCDWaitForBusyLoop

    ; busy flag not set, so return
    POP     {LR}
    BX      LR
