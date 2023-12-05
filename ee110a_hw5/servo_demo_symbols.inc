;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                           servo_demo_symbols.inc                           ;
;                             Symbols for LCD Demo                           ;
;                                Include File                                ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains the symbols used by the servo demo.
;
; Revision History:
;     



; local includes
    .include "cc26x2r/ioc_reg.inc"
    .include "cc26x2r/gpt_reg.inc"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SERVO
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Servo pins
SERVO_PWM_PIN .equ			24	; PWM control pin of the servomotor
SERVO_POS_PIN .equ			23	; analog input pin connected to the potentiometer

; Servo timer
SERVO_TIMER_BASE_ADDR .equ	GPT2_BASE_ADDR

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