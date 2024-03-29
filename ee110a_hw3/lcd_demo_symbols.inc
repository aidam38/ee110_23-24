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
; Revision History:
;     11/22/23  Adam Krivka      initial revision



; local includes
    .include "cc26x2r/ioc_reg.inc"
    .include "cc26x2r/gpt_reg.inc"



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PINS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; LCD data bus
LCD_DATA_0_PIN .equ         15
LCD_DATA_1_PIN .equ         16
LCD_DATA_2_PIN .equ         17
LCD_DATA_3_PIN .equ         18
LCD_DATA_4_PIN .equ         19
LCD_DATA_5_PIN .equ         20
LCD_DATA_6_PIN .equ         21
LCD_DATA_7_PIN .equ         22

; LCD control pins
LCD_RS_PIN .equ             6   ; register select
LCD_RW_PIN .equ             5   ; read/write
LCD_E_PIN .equ              7   ; enable



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LCD_TIMER_BASE_ADDR .equ    GPT1_BASE_ADDR