;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            EE 110a - Homework #3                           ;
;                                 LCD Demo                                   ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This demo contains a test loop which displays a series of characters and
; strings on the LCD.  The test loop is implemented using a table of strings
; and a loop which iterates through the table.  The functions used to interact
; with the LCD are defined in the folder "lcd".
;
; Revision History: 


; local include files
    .include "lcd_demo_symbols.inc"
    .include "constants.inc"
    .include "macros.inc"

    .include "cc26x2r/ioc_macros.inc"
    .include "cc26x2r/store_event_handler.inc"
    .include "cc26x2r/gpt_reg.inc"
    .include "cc26x2r/gpio_reg.inc"
    .include "cc26x2r/cpu_scs_reg.inc"

; import symbols from other files
    .ref PeriphPowerInit
    .ref GPIOClockInit
    .ref StackInit
    .ref GPTClockInit

    .ref LCDInit
    .ref Display


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .text


hello_world:   .cstring "CCCC"
adam_krivka:   .cstring "cccc"

; test cases table
	.align 4
LCDTestTab:
    ;     row,  col
    ;     str address
    .half 0,    0
    .word hello_world

    .half 5,    5
    .word adam_krivka

EndLCDTestTab:
    
    .global ResetISR ; exposing the program entry-point
ResetISR:
main:
    BL          PeriphPowerInit           ; turn on peripheral power domain
    BL          GPIOClockInit             ; turn on GPIO clock
    BL          GPTClockInit              ; turn on GPT clock
    BL          StackInit                 ; initialize stack in SRAM

    ; configure IO pins
    IOCInit

    ; data pins
    IOCFG       DATA_0_PIN, DATA_PIN_CFG
    IOCFG       DATA_1_PIN, DATA_PIN_CFG
    IOCFG       DATA_2_PIN, DATA_PIN_CFG
    IOCFG       DATA_3_PIN, DATA_PIN_CFG
    IOCFG       DATA_4_PIN, DATA_PIN_CFG
    IOCFG       DATA_5_PIN, DATA_PIN_CFG
    IOCFG       DATA_6_PIN, DATA_PIN_CFG
    IOCFG       DATA_7_PIN, DATA_PIN_CFG

    ; control pins
    IOCFG       RS_PIN, RS_PIN_CFG      ; RS pin
    IOCFG       RW_PIN, RW_PIN_CFG      ; RW pin
    IOCFG       E_PIN, E_PIN_CFG        ; E pin

    ; enable output for data pins
    MOV32       R1, GPIO_BASE_ADDR
    MOV32		R0, ((11111111b << DATA_0_PIN ) | (1 << RW_PIN) | (1 << RS_PIN) | (1 << E_PIN))
    STR         R0, [R1, #GPIO_DOE_OFFSET]

    ; test
    STREG		(0xFF << DATA_0_PIN), R1, GPIO_DOUT_OFFSET


    BL          LCDInit                 ; initialize keypad

; main loop
	ADR         R4, LCDTestTab          ; load address of test table
	ADR         R5, EndLCDTestTab       ; load address of end of test table
Loop:
    ; load test case
    LDRH        R0, [R4], #2            ; load row
    LDRH        R1, [R4], #2            ; load column
    LDR         R2, [R4], #4            ; load string address

    BL          Display                 ; display string

	CMP         R4, R5                  ; compare current address to end address
    BNE         Loop                    ; if not at end, loop

EndDemo:
	NOP
	B			EndDemo

