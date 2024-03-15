;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            keypad_symbols.inc                              ;
;                       Symbols for the keypad driver                        ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the symbols for the keypad driver.
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
; TIMER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; timer redefinitions
TIMER_BASE_ADDR .equ        BUTTON_TIMER_BASE_ADDR
TIMER_EXCEPTION_NUMBER .equ BUTTON_TIMER_EXCEPTION_NUMBER
TIMER_IRQ_NUMBER .equ       BUTTON_TIMER_IRQ_NUMBER

; timer configuration:
;  32-bit mode
;  time-out interrupt enabled
;  periodic mode, count down
;  load value of 47999 (for 1ms interrupt assuming 48MHz clock)
;  no prescale
TIMER_CFG .equ    GPT_CFG_32BIT
TIMER_IMR .equ    GPT_IMR_TATOIM_ENABLED
TIMER_TAMR .equ   GPT_TXMR_PERIODIC | GPT_TXMR_TXCDIR_DOWN
TIMER_CTL .equ    GPT_CTL_TAEN_ENABLED
TIMER_TAILR .equ  47999
TIMER_TAPR .equ   0x0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OTHER CONSTANS 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

DEBOUNCE_TIME .equ              20      ; number of timer interrupts a key
                                        ; must be pressed down

BOTH_BUTTONS_UP .equ            11b   ;



