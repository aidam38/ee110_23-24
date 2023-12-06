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
; WARNING: This file currently harcodes the usage of pin 24 for the PWM
; control. If one wanted to make this file more flexible, they'd have to change:
;	PWM_PIN
;	PWM_PIN_CFG (select the correct port event)
;
; WARNING: This file currently hardcodes the usage of pin 23 for the position
; analog input. If one wanted to make this file more flexible, they'd have
; to change:
;	POS_PIN
;	MUX3_MASK
;	AIODIO3_PIN
;
; NOTE: The test code uses Timer 3 for the GetServo/Display interrupt.
;
; Revision History:
;		12/5/23	Adam Krivka		initial revision



; local includes 
	.include "../cc26x2r/gpt_reg.inc"
	.include "../servo_demo_symbols.inc"



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; PINS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PWM_PIN .equ			24				; PWM Control 
POS_PIN .equ			23				; Potentiometer Position (pin 26 in AUX domain!)

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
TIMER_DISABLE .equ		GPT_CTL_TAEN_DISABLED	; timer A disable
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
; ANALOG-DIGITAL CONVERTER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ADC0_RESET .equ			AUX_ADI4_ADC0_EN | AUX_ADI4_ADC0_RESET | AUX_ADI4_ADC0_2700ns
ADC0_NORMAL .equ        AUX_ADI4_ADC0_EN | AUX_ADI4_ADC0_NORMAL | AUX_ADI4_ADC0_2700ns
ADCREF0 .equ			AUX_ADI4_ADCREF0_ENABLE | AUX_ADI4_ADCREF0_REF_ON_IDLE
MUX3_MASK .equ			00000001b	; one-hot encoding to select POS_PIN
AIODIO3_PIN .equ		2			; select pin in AIODIO3 range
ADCCTL .equ				AUX_ANAIF_ADCCTL_ENABLE | AUX_ANAIF_ADCCTL_TRIG_NO_EVENT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OTHER CONSTANTS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MIN_ANGLE .equ			90						; minimum angle in degrees (negative!)
MAX_ANGLE .equ			90						; maximum angle in degrees
ANGLE_RANGE .equ		(MAX_ANGLE + MIN_ANGLE)	; total angle range

ADC_MIN .equ			0x0000009C				; value in ADC corresponding to voltage when servo is at MIN_ANGLE
ADC_MAX .equ			0x000006EF	            ; value in ADC corresponding to voltage when servo is at MAX_ANGLE
ADC_RANGE .equ			(ADC_MAX - ADC_MIN)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TEST TIMER
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TESTTIMER_BASE_ADDR .equ	GPT3_BASE_ADDR			; test timer base address
TESTTIMER_IRQ_NUMBER .equ	GPT3A_IRQ_NUMBER		; interrupt number
TESTTIMER_EXCEPTION_NUMBER .equ GPT3A_EXCEPTION_NUMBER ; exception number
TESTTIMER_CFG .equ			GPT_CFG_2_16BIT			; only timer A
TESTTIMER_IMR .equ			GPT_IMR_TATOIM_ENABLED	; enable interrupt
TESTTIMER_ENABLE .equ		GPT_CTL_TAEN_ENABLED	; timer A enable
TESTTIMER_TAMR .equ			GPT_TXMR_PERIODIC		; periodic

; 1 s period / 1 Hz frequency
; 48 000 000 / 1 = 48 000 000
TESTTIMER_LENGTH .equ		48000000
TESTTIMER_TAILR .equ		(TESTTIMER_LENGTH & 0xFFFF)	; interval
TESTTIMER_TAPR .equ			(TESTTIMER_LENGTH >> 16)	; prescale

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OTHER TEST CONSTANTS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 1 second hold ~ 24 MHz / 1 Hz = 24 000 000
HOLD_TIME .equ              24000000

; row, col to display angle value at
DISPLAY_ROW .equ			0
DISPLAY_COL .equ			0
