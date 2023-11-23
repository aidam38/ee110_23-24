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
;   LCDWaitForNotBusy
; 



; local includes
    .include "../cc26x2r/gpio_reg.inc"
    .include "../cc26x2r/gpt_reg.inc"
    .include "../cc26x2r/ioc_reg.inc"
    .include "lcd_symbols.inc"
    .include "../std.inc"

; export functions to other files
    .def LCDWrite
    .def LCDRead
    .def LCDWaitForNotBusy


LCDConfigureForRead:
    PUSH    {LR}

    MOV32       R1, IOC_BASE_ADDR   ; prepare IOC base address

    ; reconfigure pins
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_0_PIN
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_1_PIN
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_2_PIN
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_3_PIN
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_4_PIN
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_5_PIN
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_6_PIN
    STREG       IOCFG_GENERIC_INPUT, R1, IOCFG_REG_SIZE * DATA_7_PIN

    ; disable output
    MOV32   R1, GPIO_BASE_ADDR
    LDR     R0, [R1, #GPIO_DOE_OFFSET]
    BIC     R0, #(11111111b << DATA_0_PIN)
    STR     R0, [R1, #GPIO_DOE_OFFSET]

    POP     {LR}
    BX      LR

LCDConfigureForWrite:
    PUSH    {LR}

    MOV32       R1, IOC_BASE_ADDR   ; prepare IOC base address

    ; reconfigure pins
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_0_PIN
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_1_PIN
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_2_PIN
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_3_PIN
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_4_PIN
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_5_PIN
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_6_PIN
    STREG       IOCFG_GENERIC_OUTPUT, R1, IOCFG_REG_SIZE * DATA_7_PIN

    ; enable output
    MOV32   R1, GPIO_BASE_ADDR
    LDR     R0, [R1, #GPIO_DOE_OFFSET]
    ORR     R0, #(11111111b << DATA_0_PIN)
    STR     R0, [R1, #GPIO_DOE_OFFSET]

    POP     {LR}
    BX      LR



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
; Return Values:        RS in R0, DATA in R1
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
    BEQ     LCDWriteWaitTimeOut              ; if not, wait

    ; clear interrupt
    MOVW    R2, #GPT_ICLR_TATOCINT_CLEAR
    STR     R2, [R5, #GPT_ICLR_OFFSET] ; write to interrupt clear register

    ; write RS based on argument and R/W low
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; load DOUT register
    BIC     R2, #(1 << RS_PIN)            ; clear RS
    ORR     R2, R0, LSL #RS_PIN         ; merge RS into DOUT while
                                        ; shifting to the RS bit position
    BIC     R2, #(1 << RW_PIN)          ; merge R/W = 0 into DOUT while
                                        ; shifting to the R/W bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; wait for 140ns (round_up(140 / 40ns) = 3.5 ~ 4)
    MOV     R2, #4
LCDWriteWaitSetup:
    SUBS    R2, #1      ; decrement counter
    BNE     LCDWriteWaitSetup

    ; write E high and data
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    ORR     R2, #(1 << E_PIN)          ; merge E = 1 into DOUT while
                                        ; shifting to the E bit position
    BIC     R2, #(11111111b << DATA_0_PIN) ; clear data
    ORR     R2, R1, LSL #DATA_0_PIN     ; merge data into DOUT while
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; start command timer
    STREG   TIMER_ENABLE, R5, GPT_CTL_OFFSET

    ; wait for 450ns by checking the match interrupt
LCDWriteWaitPulse:
    LDR     R2, [R5, #GPT_RIS_OFFSET]; read raw interrupt status
    TST     R2, #GPT_RIS_TAMRIS      ; check if timer A reached match
    BEQ     LCDWriteWaitPulse                ; if not, wait

    ; clear interrupt
    STREG   GPT_ICLR_TAMCINT_CLEAR, R5, GPT_ICLR_OFFSET; write to interrupt clear register

    ; write E low
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R2, #(1 << E_PIN)           ; merge E = 0 into DOUT while
                                        ; shifting to the E bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    POP     {LR, R4, R5}                ; restore LR, R4, R5
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
    MOVW    R2, #GPT_ICLR_TATOCINT_CLEAR
    STR     R2, [R5, #GPT_ICLR_OFFSET]; write to interrupt clear register

    ; write RS based on argument and R/W low
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; load DOUT register
    BIC        R2, #(1 << RS_PIN)            ; clear RS
    ORR     R2, R0, LSL #RS_PIN         ; merge RS into DOUT while
                                        ; shifting to the RS bit position
    ORR     R2, #(1 << RW_PIN)          ; merge R/W = 1 into DOUT while
                                        ; shifting to the R/W bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; wait for 140ns (round_up(140 / 40ns) = 3.5 ~ 4)
    MOV     R2, #4
LCDReadWaitSetup:
    SUBS    R2, #1      ; decrement counter
    BNE     LCDReadWaitSetup

    ; write E high
    LDR     R2, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    ORR     R2, #(1 << E_PIN)           ; merge E = 1 into DOUT while
                                        ; shifting to the E bit position
    STR     R2, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; start command timer
    STREG   TIMER_ENABLE, R5, GPT_CTL_OFFSET

    ; wait for 450ns by checking the match interrupt
LCDReadWaitPulse:
    LDR     R2, [R5, #GPT_RIS_OFFSET]; read raw interrupt status
    TST     R2, #GPT_RIS_TAMRIS      ; check if timer A reached match
    BEQ     LCDReadWaitPulse                ; if not, wait

    ; clear interrupt
    STREG   GPT_ICLR_TAMCINT_CLEAR, R5, GPT_ICLR_OFFSET; write to interrupt clear register

    ; write E low
    LDR     R0, [R4, #GPIO_DOUT_OFFSET] ; reload DOUT register
    BIC     R0, #(1 << E_PIN)           ; merge E = 0 into DOUT while
                                        ; shifting to the E bit position
    STR     R0, [R4, #GPIO_DOUT_OFFSET] ; write DOUT back to GPIO

    ; recover data
    LDR        R0, [R4, #GPIO_DIN_OFFSET]    ; read DIN
    LSR     R0, #DATA_0_PIN             ; shift data to the right
    AND     R0, #0xFF                   ; mask out the rest of the bits

LCDReadEnd:
    POP     {LR, R4, R5}                ; restore LR, R4, R5
    BX      LR                          ; return

; LCDWaitForNotBusy
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

LCDWaitForNotBusy:
    PUSH    {LR}

    ; configure pins for read
    BL      LCDConfigureForRead

LCDWaitForNotBusyLoop:
    ; read busy flag
    MOV     R0, #0
    BL      LCDRead

    ; test if busy flag is set
    TST     R0, #BUSY_FLAG_MASK
    BNE     LCDWaitForNotBusyLoop

LCDWaitForNotBusyDone:
	; configure pins for write
    BL      LCDConfigureForWrite

    ; busy flag not set, so return
    POP     {LR}
    BX      LR
