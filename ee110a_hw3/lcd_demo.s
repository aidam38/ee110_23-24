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
    .include "std.inc"

    .include "cc26x2r/gpt_reg.inc"
    .include "cc26x2r/gpio_reg.inc"
    .include "cc26x2r/cpu_scs_reg.inc"

; import symbols from other files
    .ref PeriphPowerInit
    .ref GPIOClockInit
    .ref StackInit
    .ref GPTClockInit

    .ref LCDInit
    .ref LCDTestDisplay
    .ref LCDTestDisplayChar
    .ref Display


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.text
i_love_ee: .cstring "I LOVE EE!!!"
adam: .cstring "- Adam"

    .global ResetISR ; exposing the program entry-point
ResetISR:
main:
    BL          PeriphPowerInit           ; turn on peripheral power domain
    BL          GPIOClockInit             ; turn on GPIO clock
    BL          GPTClockInit              ; turn on GPT clock
    BL          StackInit                 ; initialize stack in SRAM

    BL          LCDInit                   ; initialize LCD

    MOV32	R0, 0
    MOV32	R1, 0
    MOVA	R2, i_love_ee
    BL		Display

    MOV32	R0, 1
    MOV32	R1, 0
    MOVA	R2, adam
    BL		Display

    BL          LCDTestDisplay            ; test the Display function
    BL          LCDTestDisplayChar        ; test the DisplayChar function

EndDemo:
    NOP
    B            EndDemo

