;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                           keypad_demo_symbols.inc                          ;
;                          Symbols for Keypad Demo                           ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains the symbols used by the keypad demo.  It is included
; by the main keypad demo file.
;
; Revision History: 7/11/23 Adam Krivka     Initial revision

; chip specific symbols
    .include "cc26x2r/ioc_reg.inc"
    .include "cc26x2r/gpt_reg.inc"
    .include "cc26x2r/cpu_scs_reg.inc"
    .include "cc26x2r/gpio_reg.inc"

; pin definitions

; pins for selecting rows
ROWSEL_A_PIN .equ   8
ROWSEL_B_PIN .equ   9

ROWSEL_PIN_CFG .equ   0x0               ; default output pin

; pins for selecting columns
COLUMN_0_PIN .equ     11
COLUMN_1_PIN .equ     12
COLUMN_2_PIN .equ     13
COLUMN_3_PIN .equ     14

COLUMN_PIN_CFG .equ IO_INPUT | IO_PU    ; default input pin with pullup

; timer configuration:
;  32-bit mode
;  time-out interrupt enabled
;  periodic mode, count down
;  load value of 47999 (for 1ms interrupt assuming 48MHz clock)
;  no prescale
GPT_CFG .equ    GPT_CFG_32BIT
GPT_IMR .equ    GPT_IMR_TATOIM_ENABLED
GPT_TAMR .equ   GPT_TXMR_PERIODIC | GPT_TXMR_TXCDIR_DOWN
GPT_CTL .equ    GPT_CTL_TAEN_ENABLED

GPT_TAILR .equ  47999
GPT_TAPR .equ   0x0

; event queue parameters
QUEUE_SIZE .equ     256
