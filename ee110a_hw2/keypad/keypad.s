; R4 = *CurrentRow
; R5 = *DebounceCounter
; R6 = *PrevState
; R7 = [CurrentRow]
; R8 = [DebounceCounter]
; (except in KeypadInit)

    .include "keypad_symbols.inc"
    .include "../keypad_config.inc"
    .include "../macros.inc"

    .ref EnqueueEvent
    .ref SelectRow
    .ref ReadRow

    .def KeypadInit
    .def KeypadScanAndDebounce

; Global variables
    .data
    .align 8

CurrentRow: .byte 0
DebounceCounter: .byte 0
PrevState: .byte 0

; Code
    .text

KeypadInit:
	PUSH	{LR}

	MOVA	R0, CurrentRow
	MOV		R1, #0
	STRB	R1, [R0]

	MOVA	R0, DebounceCounter
	MOV		R1, #DEBOUNCE_TIME
	STRB	R1, [R0]

	MOVA	R0, PrevState
	MOV		R1, #1111b
	STRB	R1, [R0]

	POP 	{LR}
	BX	LR

KeypadScanAndDebounce:
    PUSH    {LR, R4, R5, R6, R7, R8}

    MOVA	R5, DebounceCounter
    LDRB	R8, [R5] ;dereference DebounceCounter

    CMP		R8, #DEBOUNCE_TIME ; 
    BNE		Debouncing		; if(DebounceCounter != DEBOUNCE_TIME)
    ;B		Scanning 		; if(DebounceCounter == DEBOUNCE_TIME)

Scanning:
    ;increment CurrentRow
    MOVA    R4, CurrentRow
    LDRB	R7, [R4] ;dereference CurrentRow
    ADD     R7, #1 ; increment CurrentRow
    AND		R7, #11b; take just lower two bytes
    STRB	R7, [R4] ;update CurrentRow
	MOV		R0, R7 ;prepare as argument
    
    ;output CurrentRow
    BL      SelectRow

Debouncing:
    BL      ReadRow ;returns CurState in R0

	MOVA	R6, PrevState
	LDRB	R1, [R6] ;dereference PrevState

	CMP		R0, R1
	BNE		NotDebouncing ; if (PrevState != CurState)	
	CMP		R0, #1111b ; or if (CurState == 1111)
	BEQ		NotDebouncing

; Note DebounceCounter is in R8
ActuallyDebouncing:
	SUBS	R8, #1 ; decrement DebounceCounter
	BMI		CounterNegative ; if (DebounceCounter < 0)
	BNE		ActuallyDebouncingEnd ; if (DebounceCounter != 0)
	;B		CounterZero ; if (DebounceCounter == 0)

; Note CurrentRow is in R7
; CurState is still in R0
CounterZero:
	EOR		R0, #1111b ;negate because buttons go to ground
	LSL		R0, #4 ;push by 4 to the left
	OR		R0, R7 ;merge with current row
	PUSH	{R0, R1, R2, R3}
	BL		EnqueueEvent
	POP		{R0, R1, R2, R3}
	B		ActuallyDebouncingEnd
CounterNegative:
	MOV		R8, #0 ;reset counter to zero so we don't overflow in the negatives

ActuallyDebouncingEnd:
	STRB	R8, [R5] ;store new DebounceCounter

	B		End
NotDebouncing:
	STRB	R0, [R6] ; update PrevState

	MOV		R8, #DEBOUNCE_TIME
	STRB	R8, [R5]

End:
    POP     {LR, R4, R5, R6, R7, R8}
    BX 	    LR
