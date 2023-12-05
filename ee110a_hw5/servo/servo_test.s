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
; none

; import functions from other files
	.ref SetServo
	.ref ReleaseServo
	.ref GetServo

; export symbols to other files
    .def TestServo

TestServoTab:
	.byte 0, -90, -89, -85, 90, 89, 58, -45, 45, -10, 10, -91, 1000000
EndTestServoTab:

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

	; set up interrupt to output GetServo to the LCD using Display
	; TODO

	ADR		R4, TestServoTab		; load address of test table
	ADR		R5, EndTestServoTab		; load address of end of test table

TestServoLoop:
	LDRB	R0, [R4], #1			; load position
	BL		SetServo				; set servo's position
	BL		ReleaseServo			; release servo

	; PUT BREAKPOINT HERE

	CMP         R4, R5				; compare current address to end address
    BNE         TestServoLoop		; if not at end, loop
    ;B          TestServoEnd

TestServoEnd:
	POP		{LR, R4, R5}			; restore return address
	BX		LR						; return
