;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            EE 110a - Homework #3                           ;
;                                 LCD Demo                                   ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains a program which demos the functionality of a 14-pin
; character LCD. The LCD functionality as well as the test loop is implemented
; the the lcd folder. This file contains the main loop which initializes the
; board and runs the tests. The tests test the functions Display and DisplayChar.
;
; Revision History: 
;     11/22/23  Adam Krivka      initial revision


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
    .ref TestDisplay
    .ref TestDisplayChar
    .ref Display


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.text
; main
;
; Description:          Main loop of the LCD Demo program. Initializes
;                       the board and the LCD, and tests the Display
;                       and DisplayChar functions.
;
; Arguments:            None.
; Return Values:        None.
; 
; Local Variables:      In nested functions...
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       Facilitated using FUNCTION_SUCCESS and FUNCTION_FAIL
;                       in called functions.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          5?
; 
; Revision History:
;     11/22/23  Adam Krivka      initial revision

    .global ResetISR                  ; exposing the program entry-point
ResetISR:
main:
    BL      PeriphPowerInit           ; turn on peripheral power domain
    BL      GPIOClockInit             ; turn on GPIO clock
    BL      GPTClockInit              ; turn on GPT clock
    BL      StackInit                 ; initialize stack in SRAM

    BL      LCDInit                   ; initialize LCD

    BL      TestDisplay            ; test the Display function
    BL      TestDisplayChar        ; test the DisplayChar function

EndDemo:
    NOP
    B       EndDemo

