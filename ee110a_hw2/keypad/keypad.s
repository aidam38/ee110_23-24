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
    PUSH    {LR}

    MOVA	R0, DebounceCounter
    LDRB	R1, [R0]

    CMP		R1, #DEBOUNCE_TIME
    BNE		Debouncing
    ;B		Scanning

Scanning:
    ;increment CurrentRow
    MOVA    R0, CurrentRow
    LDRB	R2, [R0]
    ADD     R2, #1 ; we don't care about the higher bytes
    AND		R2, #11b
    STRB	R2, [R0]
    
    ;output CurrentRow
    BL      SelectRow

Debouncing:
    BL      ReadRow ;R0 is CurState

	MOVA	R2, PrevState
	LDRB	R3, [R2]

	CMP		R0, R3
	BNE		NotDebouncing
	CMP		R0, #1111b
	BEQ		NotDebouncing

ActuallyDebouncing:
	MOVA	R2, DebounceCounter
    LDRB	R1, [R2]
	SUBS	R1, #1
	BMI		CounterNegative
	BNE		ActuallyDebouncingEnd
	;B		CounterZero

CounterZero:
	MOVA	R1, CurrentRow
	LDR		R1, [R1]
	EOR		R0, #1111b
	LSL		R0, R1
	PUSH	{R0, R1, R2, R3}
	BL		EnqueueEvent
	POP		{R0, R1, R2, R3}
	MOV		R1, #0
	B		ActuallyDebouncingEnd
CounterNegative:
	MOV		R1, #0

ActuallyDebouncingEnd:
	STRB	R1, [R2] ;store new DebounceCounter


	B		End
NotDebouncing:
	MOVA 	R2, PrevState
	STRB	R0, [R2] ; update PrevState

	MOVA	R0, DebounceCounter
	MOV		R1, #DEBOUNCE_TIME
	STRB	R1, [R0]

End:
    POP     {LR}
    BX 	    LR
