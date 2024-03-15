;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  hw.inc                                    ;
;                      Hardware configuraiton symbols                        ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains hardware configuration for the current project. 
; It is included by the main application file and other files.
; Modify as needed.

; local includes 
    .include "cc26x2r/gpt_reg.inc"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Button
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Keypad timer
BUTTON_TIMER_BASE_ADDR .equ         GPT0_BASE_ADDR
BUTTON_TIMER_EXCEPTION_NUMBER .equ  GPT0A_EXCEPTION_NUMBER
BUTTON_TIMER_IRQ_NUMBER .equ        GPT0A_IRQ_NUMBER
