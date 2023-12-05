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
;     


; local includes
	.include "../std.inc"
	.include "../cc26x2r/gpio_reg.inc"
	.include "../cc26x2r/gpt_reg.inc"
	.include "servo_symbols.inc"

; export functions to other files
	.def InitServo
	.def SetServo
	.def ReleaseServo
	.def GetServo

; InitServo
;
; Description:          
;
; Arguments:            pos in R0
; Return Values:        success/fail in R0.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          
; 
; Revision History:
;		

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

	STREG	TIMER_ENABLE, R1, GPT_CTL_OFFSET ; start the timer

	POP		{LR}						; restore return address
	BX		LR							; return




; SetServo
;
; Description:          
;
; Arguments:            pos in R0
; Return Values:        success/fail in R0.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          
; 
; Revision History:
;		

SetServo:
	PUSH	{LR}					; save return address and used registers

	CMP		R0, #MIN_ANGLE			; check if pos < MIN_ANGLE
	BLT		SetServoFail			; if not, fail

	CMP		R0, #MAX_ANGLE			; check if pos > MAX_ANGLE
	BGT		SetServoFail			; if not, fail
	;B		SetServoInputGood

SetServoInputGood:
; Convert pos to a timer match value
	MOV32	R1, MIN_ANGLE
	ADD		R0, R1					; [MIN_ANGLE, MAX_ANGLE] 	=> [0, ANGLE_RANGE]
	
	MOV32	R1, (ANGLE_RANGE / TIMER_MATCH_RANGE)
	SDIV	R0, R1					;							=> [0, TIMER_MATCH_RANGE]

	MOV32	R1, TIMER_MATCH_MIN
	SUB		R0, R1					;							=> [TIMER_MATCH_MIN, TIMER_MATCH_MAX]

; Change PWM pulse width
	MOV32	R1, TIMER_BASE_ADDR		; prepare timer base address
	STR		R0, [R1, GPT_TAMATCHR_OFFSET] ; write to Timer A Match register
	B		SetServoSuccess			; return success

SetServoFail:
	MOV		R0, #FUNCTION_FAIL		; prepare FAIL return value
	B		SetServoEnd

SetServoSuccess:
	MOV		R0, #FUNCTION_SUCCESS	; prepare SUCCESS return value
	;B		SetServoEnd

SetServoEnd:
	POP		{LR}					; restore return address
	BX		LR						; return



; ReleaseServo
;
; Description:          
;
; Arguments:            None.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          
; 
; Revision History:
;		

ReleaseServo:
	PUSH	{LR}					; save return address and used registers
	POP		{LR}					; restore return address
	BX		LR						; return



; GetServo
;
; Description:          
;
; Arguments:            None.
; Return Values:        pos in R0.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          
; 
; Revision History:
;		

GetServo:
	PUSH	{LR}					; save return address and used registers
	POP		{LR}					; restore return address
	BX		LR						; return