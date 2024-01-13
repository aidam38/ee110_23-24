;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                               conversions.s                                ;
;                          Various type conversions                          ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This files contains functions that convert between various types:
;   i16ToString - convert a 16-bit integer to a string
;
; Revision History:

; local includes
;  none

; export functions to other files
	.def i16ToString



; i16ToString
;
; Description:          Converts a 16-bit integer to a string.
;
; Arguments:            R0 = integer to convert, R1 = string buffer pointer (must
;						exactly 8 bytes).
; Return Values:        None (string in buffer pointed by R1)
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          13
; 
; Revision History:

i16ToString:
	PUSH	{LR, R4, R5, R6}			; save return address and used registers

; save variables
	MOV		R4, R0						; integer to convert
	MOV		R5, R1						; string buffer pointer

; clean buffer (set to zeros)
	MOV		R2, #0
	STR		R2, [R1], #4				; set first word to zero
	STR		R2, [R1], #4				; set second word to zero

; create zeroed local buffer of 8 bytes on the stack
	STR		R2, [R13, #-4]!
	STR		R2, [R13, #-4]!
	MOV		R6, R13						; store its top in a variable

	MOV		R3, #10						; prepare base 10
i16ToStringConversionLoop:
	SDIV	R0, R4, R3					; R0 = R4 / 10 (rounded towards zero)
	MLS		R1, R0, R3, R4				; R1 = R4 - R0*10 (R1 = R4 % 10)

	ADD		R1, #48						; convert to ASCII

	STRB	R1, [R6], #1				; store in stack buffer

	CMP		R0, #0						; if result was zero, this was the last digit
	BEQ		i16ToStringConversionLoopDone

	MOV		R4, R0						; update number to convert

	B 		i16ToStringConversionLoop	; loop

i16ToStringConversionLoopDone:
	; TODO: check if negative
	B		i16ToStringPositive

i16ToStringNegative:
	MOV		R1, #45
	STRB	R1, [R6], #1				; add minus sign to buffer
	;B		i16ToStringPositive

i16ToStringPositive:
	SUB		R6, #1
	;B		i16ToStringCopy

i16ToStringCopyLoop:
	LDRB	R1, [R6], #-1				; load byte from stack buffer
	STRB	R1, [R5], #1				; store byte in string buffer
	CMP		R6, R13						; check if below the top of stack
	BGE		i16ToStringCopyLoop
	;B		i16ToStringCopyDone

i16ToStringCopyDone:
	ADD		R13, #8
	POP		{LR, R4, R5, R6}			; restore return address and used registers
	BX		LR							; return
