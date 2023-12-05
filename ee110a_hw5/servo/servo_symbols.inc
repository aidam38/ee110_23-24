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
;	EVENT_TIMERCAPTSEL_OFFSET
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
TIMER_TAMR .equ			GPT_TXMR_PERIODIC | GPT_TXMR_TXCDIR_DOWN | GPT_TXMR_TXAMS_PWM | GPT_TXMR_TXCINTD_DISABLED | GPT_TXMR_TXPLO_HIGH

; 20 ms period / 50 Hz frequency
; 48 000 0000 / 50 = 960 000
TIMER_PULSE_WIDTH .equ  960000
TIMER_TAILR .equ		(TIMER_PULSE_WIDTH & 0xFFFF)		; interval
TIMER_TAPR .equ			(TIMER_PULSE_WIDTH >> 16)			; prescale

; min pulse = 0.5 ms -> 0.001 * 48000000 ~= 48 000
TIMER_MATCH_MIN .equ	24000
TIMER_TAMATCHR_MIN .equ	((TIMER_PULSE_WIDTH - TIMER_MATCH_MIN) & 0xFFFF)
TIMER_TAPMR_MIN .equ	((TIMER_PULSE_WIDTH - TIMER_MATCH_MIN) >> 16)

; max pules = 2 ms -> 0.0025 * 48000000 ~= 96 000
TIMER_MATCH_MAX .equ	120000
TIMER_TAMATCHR_MAX .equ	((TIMER_PULSE_WIDTH - TIMER_MATCH_MAX) & 0xFFFF)
TIMER_TAPMR_MAX .equ    ((TIMER_PULSE_WIDTH - TIMER_MATCH_MAX) >> 16)

TIMER_MATCH_RANGE .equ	(TIMER_MATCH_MAX - TIMER_MATCH_MIN)


EVENT_TIMERCAPTSEL_OFFSET .equ 	EVENT_GPT2ACAPTSEL_OFFSET ; Event fabric capture select
									; hardcoded to timer 2 right now

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OTHER CONSTANTS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MIN_ANGLE .equ			60						; minimum angle in degrees (negative!)
MAX_ANGLE .equ			60						; maximum angle in degrees
ANGLE_RANGE .equ		((MAX_ANGLE + MIN_ANGLE))	; total angle range
