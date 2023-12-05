;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            EE 110a - Homework #5                           ;
;                                 Servo Demo                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains a program which demos ...
;
; Revision History: 
;     


; local include files
    .include "servo_demo_symbols.inc"
    .include "std.inc"

    .include "cc26x2r/gpt_reg.inc"
    .include "cc26x2r/gpio_reg.inc"
    .include "cc26x2r/cpu_scs_reg.inc"

; import symbols from other files
    .ref PeriphPowerInit
    .ref GPIOClockInit
    .ref StackInit
    .ref GPTClockInit

	.ref Ini
    .ref LCDInit
    .ref TestServo


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.text
; main
;
; Description:          Main loop of the Servo Demo program. Initializes
;                       the board, the Servo, and the LCD, and tests the 
;                       Servo using functions from servo/servo_test.s
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

; initialize Servo and LCD
	BL		InitServo					; initialize Servo
    BL      LCDInit                   ; initialize LCD

; test Servo
    BL      TestServo               ; test the Display function

EndDemo:
    NOP
    B       EndDemo

