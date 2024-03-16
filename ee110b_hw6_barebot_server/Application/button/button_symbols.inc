;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            button_symbols.inc                              ;
;                       Symbols for the button driver                        ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the symbols for the button driver.
; 
; Revision History:
;       11/7/23  Adam Krivka      initial revision
;       3/6/24   Adam Krivka      fixed formatting


; local includes
    .include "../cc26x2r/ioc_reg.inc"
    .include "../cc26x2r/gpt_reg.inc"
    .include "../cc26x2r/gpio_reg.inc"
    .include "../cc26x2r/cpu_scs_reg.inc"
    .include "../hw.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GPIO
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; pins for selecting columns
BUTTON_0_PIN .equ       13
BUTTON_1_PIN .equ       14

BUTTON_CFG .equ (IO_PU | IO_INPUT)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OTHER CONSTANS 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DEBOUNCE_TIME .equ              20      ; number of timer interrupts a key
                                        ; must be pressed down

BOTH_BUTTONS_UP .equ            11b   ;



