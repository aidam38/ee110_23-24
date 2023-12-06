;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   servo.s                                  ;
;                                 Servo Driver                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains the initialization and operation code of a servomotor,
; using the PWM interface. This implementation also includes the capability
; of reading the servo's current position by plugging into its internal
; potentiometer measuring the position.
; 
; This file defines functions:
;		SetServo(pos) - sets the position of the servo
;		ReleaseServo() - release servo from holding
;		GetServo() - get servo's current position
; 
; Revision History:
;		12/5/23	Adam Krivka		initial revision



; local includes
	.include "../std.inc"
	.include "../cc26x2r/gpio_reg.inc"
	.include "../cc26x2r/gpt_reg.inc"
	.include "../cc26x2r/event_reg.inc"
	.include "../cc26x2r/aux_reg.inc"
	.include "servo_symbols.inc"

; export functions to other files
	.def InitServo
	.def SetServo
	.def ReleaseServo
	.def GetServo

; InitServo
;
; Description:          Initializes the servo pins, the PWM timer, and
;						the Analog-to-Digital converter.
;
; Arguments:            None.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
; 
; Revision History:	
;		12/5/23	Adam Krivka		initial revision

InitServo:
	PUSH	{LR}						; save return address and used registers

; Initialize pins
	MOV32	R1, IOC_BASE_ADDR			; prepare IOC base address
	STREG	PWM_PIN_CFG, R1, IOCFG_REG_SIZE * PWM_PIN ; PWM pin
	STREG	POS_PIN_CFG, R1, IOCFG_REG_SIZE * POS_PIN ; POS pin

; Enable output for PWM pin
	MOV32	R1, GPIO_BASE_ADDR			; prepare GPIO base address
	LDR		R0, [R1, #GPIO_DOE_OFFSET]	; load DOE registers
	ORR		R0, #(1 << PWM_PIN)			; merge enable value for PWM pin
	STR		R0, [R1, #GPIO_DOE_OFFSET]	; write back

; Set up timer
	MOV32	R1, TIMER_BASE_ADDR			; prepare timer base address
	STREG	TIMER_CFG, R1, GPT_CFG_OFFSET
	STREG	TIMER_TAMR, R1, GPT_TAMR_OFFSET
	STREG	TIMER_TAILR, R1, GPT_TAILR_OFFSET
	STREG	TIMER_TAPR, R1, GPT_TAPR_OFFSET

	; start in MIN_ANGLE position
	STREG	TIMER_TAMATCHR_MIN, R1, GPT_TAMATCHR_OFFSET
	STREG	TIMER_TAPMR_MIN, R1, GPT_TAPMR_OFFSET

	STREG	TIMER_ENABLE, R1, GPT_CTL_OFFSET ; start the timer

; Map timer output to pin
	MOV32	R1, EVENT_BASE_ADDR			; prepare EVENT base address
	STREG	EVENT_GPTXCAPTSEL_PORT, R1, EVENT_TIMERCAPTSEL_OFFSET ; select output for timer

; Set up ADC
	; Enable ADC clock
	MOV32	R1, AUX_SYSIF_BASE_ADDR		; prepare system interface base address
	STREG	AUX_SYSIF_ADCCLKCTL_ENABLE, R1, AUX_SYSIF_ADCCLKCTL_OFFSET ; enable clock

InitServoADCClockLoop:
	LDR		R0, [R1, #AUX_SYSIF_ADCCLKCTL_OFFSET]	; read ADC Clock Control register
	TST		R0, #AUX_SYSIF_ADCCLKCTL_ACK_ENABLE		; check ACK flag
	BEQ		InitServoADCClockLoop					; if not set, loop
	;B		InitServoADCContinue

InitServoADCContinue:
	; Select input pin, configure ADC, and enable reference module
	MOV32	R1, AUX_ADI4_BASE_ADDR		; prepare aux master base address
	STREG	ADC0_RESET, R1, AUX_ADI4_ADC0_OFFSET	; enable ADC in reset mode
	STREG	ADCREF0, R1, AUX_ADI4_ADCREF0_OFFSET ; enable reference module
	STREG	MUX3_MASK, R1, AUX_ADI4_MUX3_OFFSET ; select POS_PIN
	STREG	ADC0_NORMAL, R1, AUX_ADI4_ADC0_OFFSET	; put ADC in normal mode
	STREG	ADCREF0, R1, AUX_ADI4_ADCREF0_OFFSET ; enable reference module again?

	; Allow input
	MOV32	R1, AUX_AIODIO3_BASE_ADDR	; prepare AIO/DIO base address
	STREG	(AUX_AIODIO_IOMODE_INPUT << AIODIO3_PIN * AUX_AIODIO_IOMODE_IOSIZE), R1, AUX_AIODIO_IOMODE_OFFSET ; write to AIODIO3_PIN IO

	; Enable ADC, disable start events (other than manual trigger)
	MOV32	R1, AUX_ANAIF_BASE_ADDR		; prepare analog interface base address
	STREG	ADCCTL, R1, AUX_ANAIF_ADCCTL_OFFSET

	POP		{LR}						; restore return address
	BX		LR							; return



; SetServo
;
; Description:          Set servo position to pos, which should be a signed
;						integer in the range [-MIN_ANGLE, MAX_ANGLE].
;
; Arguments:            pos in R0
; Return Values:        success/fail in R0.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       If pos is outside [-MIN_ANGLE, MAX_ANGLE], don't do
;						anything to the PWM signal and return FUNCTION_FAIL
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          2
; 
; Revision History:
;		12/5/23	Adam Krivka		initial revision

SetServo:
	PUSH	{LR, R4}				; save return address and used registers

	MOV		R4, R0					; save pos as local variable

	CMN		R4, #MIN_ANGLE			; check if pos < MIN_ANGLE
	BLT		SetServoFail			; if not, fail

	CMP		R4, #MAX_ANGLE			; check if pos > MAX_ANGLE
	BGT		SetServoFail			; if not, fail
	;B		SetServoInputGood

SetServoInputGood:
; Enable timer
	MOV32	R1, TIMER_BASE_ADDR			; prepare timer base address
	STREG	TIMER_ENABLE, R1, GPT_CTL_OFFSET ; stop the timer

; Convert pos to a timer match value
	MOV32	R1, MIN_ANGLE
	ADD		R4, R1				; [MIN_ANGLE, MAX_ANGLE] => [0, ANGLE_RANGE]
	
	MOV32	R1, ANGLE_RANGE		;			invert
	SUB		R4, R1, R4

	MOV32	R1, TIMER_MATCH_RANGE
	MUL		R4, R4, R1			;			=> [0, ANGLE_RANGE * TIMER_MATCH_RANGE]

	MOV32	R1, ANGLE_RANGE
	SDIV	R4, R4, R1			;			=> [0, TIMER_MATCH_RANGE]

	MOV32	R1, TIMER_MATCH_MIN
	ADD		R4, R1				;			=> [TIMER_MATCH_MIN, TIMER_MATCH_MAX]

; Change PWM pulse width
	; map value for down counter
	MOV32	R1, TIMER_PULSE_WIDTH
	SUB		R4, R1, R4

	; split match value into interval and prescale
	MOV		R1, #0xFFFF
	AND		R2, R4, R1				; interval
	LSR		R3, R4, #16				; prescale

	MOV32	R1, TIMER_BASE_ADDR		; prepare timer base address
	STR		R2, [R1, #GPT_TAMATCHR_OFFSET] ; write to Timer A Match register
	STR		R3, [R1, #GPT_TAPMR_OFFSET] ; write to Timer A Match prescale register
	B		SetServoSuccess			; return success

SetServoFail:
	MOV		R0, #FUNCTION_FAIL		; prepare FAIL return value
	B		SetServoEnd

SetServoSuccess:
	MOV		R0, #FUNCTION_SUCCESS	; prepare SUCCESS return value
	;B		SetServoEnd

SetServoEnd:
	POP		{LR, R4}				; restore return address
	BX		LR						; return



; ReleaseServo
;
; Description:          Release servo by turning of the PWM timer (pin
;						should go low).
;
; Arguments:            None.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
; 
; Revision History:
;		12/5/23	Adam Krivka		initial revision

ReleaseServo:
	PUSH	{LR}					; save return address and used registers

; Disable timer 
	MOV32	R1, TIMER_BASE_ADDR			; prepare timer base address
	STREG	TIMER_DISABLE, R1, GPT_CTL_OFFSET ; stop the timer

	POP		{LR}					; restore return address
	BX		LR						; return



; GetServo
;
; Description:          Get the current servo position using the Analog-to-Digital
;						interface.
;
; Arguments:            None.
; Return Values:        pos in R0.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
; 
; Revision History:
;		12/5/23	Adam Krivka		initial revision

GetServo:
	PUSH	{LR}					; save return address and used registers

; Trigger ADC conversion
	STREG	AUX_ANAIF_ADCTRIG_TRIG, R1, AUX_ANAIF_ADCTRIG_OFFSET ; trigger

; Wait for conversion to finish...
GetServoWait:
	LDR		R0, [R1, #AUX_ANAIF_ADCFIFOSTAT_OFFSET]	; read FIFO status
	TST		R0, #AUX_ANAIF_ADCFIFOSTAT_EMPTY		; check if empty
	BNE		GetServoWait							; if empty, loop
	;B		GetServoRead

; Read servo position using ADC
GetServoRead:
	LDR		R0, [R1, #AUX_ANAIF_ADCFIFO_OFFSET]		; read FIFO, get 12-bit value
	MOV32	R1, AUX_ANAIF_ADCFIFO_MASK				; mask out higher bits in
	AND		R0, R1									; case they're mangled
	;B		GetServoConvert

; Covert ADC output to angle in degrees
GetServoConvert:
	MOV32	R1, ADC_MIN			; [ADC_MIN, ADC_MAX] => [0, ADC_RANGE]
	SUB		R0, R1

	MOV32	R1, ADC_RANGE		;					invert
	SUB		R0, R1, R0

	MOV32	R1, ANGLE_RANGE		; 					=> [0, ADC_RANGE*ANGLE_RANGE]
	MUL		R0, R1

	MOV32	R1, ADC_RANGE		;					=> [0, ANGLE_RANGE]
	SDIV	R0, R0, R1

	MOV32	R1, MIN_ANGLE		;					=> [MIN_ANGLE, MAX_ANGLE]
	SUB		R0, R1
	
	;B		GetServoReturn

GetServoReturn:
	POP		{LR}					; restore return address
	BX		LR						; return
