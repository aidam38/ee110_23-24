;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                             lcd_demo_symbols.inc                           ;
;                             Symbols for LCD Demo                           ;
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


; pin definitions

; data pins
DATA_0_PIN .equ         15
DATA_1_PIN .equ         16
DATA_2_PIN .equ         17
DATA_3_PIN .equ         18
DATA_4_PIN .equ         19
DATA_5_PIN .equ         20
DATA_6_PIN .equ         21
DATA_7_PIN .equ         22

DATA_PIN_CFG .equ       IO_INPUT ; both an output and input pin

; pins for controlling the lcd
RS_PIN .equ             6   ; register select
RW_PIN .equ             5  ; read/write
E_PIN .equ              7   ; enable

RS_PIN_CFG .equ         0x0 ; just output
RW_PIN_CFG .equ         0x0 ; just output
E_PIN_CFG .equ         0x0 ; just output
