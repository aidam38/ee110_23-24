
    .include "cc26x2r/gpt_reg.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; LCD
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

; LCD timer
LCD_TIMER_BASE_ADDR .equ    GPT1_BASE_ADDR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Keypad
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Keypad timer
KEYPAD_TIMER_BASE_ADDR .equ GPT0_BASE_ADDR
KEYPAD_TIMER_EXCEPTION_NUMBER .equ GPT0A_EXCEPTION_NUMBER
KEYPAD_TIMER_IRQ_NUMBER .equ GPT0A_IRQ_NUMBER

; pins for selecting rows
KEYPAD_ROWSEL_A_PIN .equ   8
KEYPAD_ROWSEL_B_PIN .equ   9

; pins for selecting columns
KEYPAD_COLUMN_0_PIN .equ     11
KEYPAD_COLUMN_1_PIN .equ     12
KEYPAD_COLUMN_2_PIN .equ     13
KEYPAD_COLUMN_3_PIN .equ     14
