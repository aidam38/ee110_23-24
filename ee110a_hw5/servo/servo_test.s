;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                servo_test.s                                ;
;                             Servomotor Test Code                           ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains the function:
;   TestServo
; which tests Servo functionality, defined in servo.s.
; 
; Revision History: 
;     



; local includes
	.include "../std.inc"
	.include "../cc26x2r/gpio_reg.inc"
	.include "../cc26x2r/gpt_reg.inc"
	.include "../cc26x2r/event_reg.inc"
	.include "../cc26x2r/aux_reg.inc"
	.include "../cc26x2r/cpu_scs_reg.inc"
	.include "servo_symbols.inc"

; import functions from other files
	.ref SetServo
	.ref ReleaseServo
	.ref GetServo
	.ref Display
	.ref AngleToAscii

; export symbols to other files
    .def TestServo

	.align 4		; ADR expects a word-aligned address
TestServoTab:
	.word 0, -60, 60, -30, 30, -15, 15, 0, 255
EndTestServoTab:


; TestServoEventHandler
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

TestServoEventHandler:
	PUSH	{LR, R4}		; save return address and used registers

	BL		GetServo		; call GetServo

	BL		AngleToAscii	; convert signed integer to ascii
	MOV		R4, R0			; save string in R4

	MOV		R0, #0			; push all zeros to stack (so that string will be null-delimited)
	PUSH	{R0}			

	PUSH	{R4}			; push string to stack
	MOV		R2, R13			; stack pointer should point to the start of string

	MOV		R0, #DISPLAY_ROW; prepare row argument
	MOV		R1, #DISPLAY_COL; prepare column argument

	BL		Display			; display

; Clear interrupt
	MOV32	R1, TESTTIMER_BASE_ADDR	; prepare timer base address
	STREG	GPT_ICLR_TATOCINT_CLEAR, R1, GPT_ICLR_OFFSET	; clear Timer A Time-out bit

	POP		{LR, R4}		; restore return address
	BX		LR				; return


; TestServo
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

TestServo:
	PUSH	{LR, R4, R5}			; save return address and used registers

; Set up interrupt to output GetServo to the LCD using Display
	MOV32	R1, TESTTIMER_BASE_ADDR	; prepare test timer base address
	STREG	TESTTIMER_CFG, R1, GPT_CFG_OFFSET
	STREG	TESTTIMER_IMR, R1, GPT_IMR_OFFSET
	STREG	TESTTIMER_TAMR, R1, GPT_TAMR_OFFSET
	STREG	TESTTIMER_TAILR, R1, GPT_TAILR_OFFSET
	STREG	TESTTIMER_TAPR, R1, GPT_TAPR_OFFSET

	STREG	TESTTIMER_ENABLE, R1, GPT_CTL_OFFSET	; enable timer

	; Set up interrupt in CPU
	MOV32	R1, SCS_BASE_ADDR
	STREG	(0x1 << TESTTIMER_IRQ_NUMBER), R1, SCS_NVIC_ISER0_OFFSET ; enable interrupt
	LDR		R0, [R1, #SCS_VTOR_OFFSET] 		; load VTOR address
	MOVA	R1, TestServoEventHandler		; load event handler address
	STR		R1, [R0, #(BYTES_PER_WORD * TESTTIMER_IRQ_NUMBER)] ; store event handler

	ADR		R4, TestServoTab		; load address of test table
	ADR		R5, EndTestServoTab		; load address of end of test table

TestServoLoop:
	LDR		R0, [R4], #4			; load position
	BL		SetServo				; set servo's position
	BL		ReleaseServo			; release servo

	; PUT BREAKPOINT HERE

	CMP		R4, R5					; compare current address to end address
    BNE		TestServoLoop			; if not at end, loop
    ;B		TestServoEnd

TestServoEnd:
	POP		{LR, R4, R5}			; restore return address
	BX		LR						; return
