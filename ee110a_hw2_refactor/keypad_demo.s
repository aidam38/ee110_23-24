;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            EE 110a - Homework #2                           ;
;                                   Keypad                                   ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file includes the entry point and main loop for the keypad demo program
; for EE 110a Homework #2.  The program first initializes the board (peripheral
; power, clocks, stack, vector table), then configures the GPIO pins according
; to the schematic/wiring, and finally sets up the GPT0A timer to trigger an
; interrupt every 1ms.  The interrupt handler then calls the KeypadScanAndDebounce
; function, which scans the keypad and debounces the keys. If a key is success-
; fully debounce, it is stored in the event queue.  This program relies solely
; on the interrupt event handler - the main loop does nothing.
;
; Revision History: 10/27/23 Adam Krivka initial revision


; local include files
    .include "std.inc"

    .include "cc26x2r/gpt_reg.inc"
    .include "cc26x2r/gpio_reg.inc"
    .include "cc26x2r/cpu_scs_reg.inc"

; import symbols from other files
    .ref PeriphPowerInit
    .ref GPIOClockInit
    .ref StackInit
    .ref MoveVecTable
    .ref GPTClockInit
    .ref KeypadInit
        .ref KeypadRegisterHwi




    .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; exposing the program entry-point
    .global ResetISR
ResetISR:
main:
    BL        PeriphPowerInit         ; turn on peripheral power domain
    BL        GPIOClockInit             ; turn on GPIO clock
    BL        GPTClockInit            ; turn on GPT clock
    BL        StackInit                ; initialize stack in SRAM
    BL        MoveVecTable            ; initialize vector table in SRAM

	BL	      KeypadRegisterHwi
    BL        KeypadInit                ; initialize keypad

; main loop - does nothing, everything happens in event handler
Loop:
    NOP
    B        Loop

