;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                             servo_symbols.inc                              ;
;                               Servo Symbols                                ;
;                               Include File                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains symbols to configure a servomotor.
;
; WARNING: This file currently hardcodes the usage of Timer 2 for the Servo.
; If one wanted to make this file more flexible, they'd have to change the
; following constants to be derived from a SERVO_TIMER [0-3]:
;	PWM_PIN_CFG (select the correct PORT_EVENT)
;	TIMER_BASE_ADDR
;
; Revision History:
;     

; local includes 
	.include "../cc26x2r/gpt_reg.inc"
	.include "../servo_demo_symbols.inc"



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PINS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PWM_PIN .equ			SERVO_PWM_PIN	; PWM Control 
POS_PIN .equ			SERVO_POS_PIN	; Potentiometer Position

; PWM pin should received signal from the timer
PWM_PIN_CFG .equ		IO_PORT_ID_EVENT4 | IO_NOPUPD

; POS pin should receive signal from the ADC
POS_PIN_CFG .equ		IO_PORT_ID_AUXIO | IO_NOPUPD | IO_INPUT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TIMER_BASE_ADDR .equ	GPT2_BASE_ADDR			; timer base address
TIMER_CFG .equ			GPT_CFG_2_16BIT			; only timer A
TIMER_ENABLE .equ		GPT_CTL_TAEN_ENABLED	; timer A enable
TIMER_TAMR .equ			GPT_TXMR_PERIODIC | GPT_TXMR_TXCDIR_UP | GPT_TXMR_TXAMS_PWM | GPT_TXMR_TXCINTD_DISABLED

; 20 ms period
; 48 MHz / (256 * (1000/20)) = 3750
TIMER_TAILR .equ		3750					; interval = 3750
TIMER_TAPR .equ			255						; prescale = 256

; min pulse = 0.5 ms -> 0.0005 * 48000000 / 256 ~= 94
; max pules = 2.5 ms -> 0.0025 * 48000000 / 256 ~= 469
TIMER_MATCH_MIN .equ	94
TIMER_MATCH_MAX .equ	469

TIMER_MATCH_RANGE .equ	(TIMER_MATCH_MAX - TIMER_MATCH_MIN)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OTHER CONSTANTS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MIN_ANGLE .equ			-90						; minimum angle in degrees
MAX_ANGLE .equ			90						; maximum angle in degrees
ANGLE_RANGE .equ		(MAX_ANGLE - MIN_ANGLE)	; total angle range