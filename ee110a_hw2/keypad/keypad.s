; R4 = *CurrentRow
; R5 = *DebounceCounter
; R6 = *PrevState
; R7 = [CurrentRow]
; R8 = [DebounceCounter]
; (except in KeypadInit)

    .include "keypad_symbols.inc"
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

KeypadScanAndDebounceInit:
	;load current row
    MOVA    R4, CurrentRow
    LDRB	R7, [R4] ;dereference CurrentRow

	;load DebounceCounter
    MOVA	R5, DebounceCounter
    LDRB	R8, [R5] ;dereference DebounceCounter

    CMP		R8, #DEBOUNCE_TIME ; 
    BNE		Read		; if(DebounceCounter != DEBOUNCE_TIME)
    ;B		Scan 		; if(DebounceCounter == DEBOUNCE_TIME)

Scan:
    ;increment CurrentRow
    ADD     R7, #1 ; increment CurrentRow
    AND		R7, #CURRENT_ROW_MASK; take just lower two bytes
    STRB	R7, [R4] ;update CurrentRow
    
    ; call SelectRow(CurrentRow)
	MOV		R0, R7 ;prepare as argument
    BL      SelectRow ;SelectRow(CurrentRow)

Read:
    BL      ReadRow ;returns CurState in R0
	;B		Compare

Compare:
	;load PrevState
	MOVA	R6, PrevState
	LDRB	R1, [R6] ;dereference PrevState

	; if PrevState != CurState, reset debounce state
	CMP		R0, R1
	BNE		ResetDebounce

	;if PrevState == CurState AND CurState == 1111, nothing is happening
	;no need to reset, just end the function
	CMP		R0, #COLUMN_ALL_KEYS_UP
	BEQ		KeypadScanAndDebounceEnd

	; if PrevState == CurState AND CurState != 1111 (meaning key is continuously
	; pressed), debounce
	;B		Debounce

; Note DebounceCounter is in R8
Debounce:
	SUBS	R8, #1 ; decrement DebounceCounter

	; if DebounceCounter < 0
	BMI		DebounceCounterNegative 

	; if DebounceCounter > 0
	BNE		DebounceEnd 

	; if DebounceCounter == 0
	;B		DebounceCounterZero 

; Note CurrentRow is in R7
; CurState is still in R0
DebounceCounterZero:
	;prepare event vector
	;convert one-hot encoding to binary, look up table is easiest
	;keep in mind order of columns is by default reversed

JumpTable:
	CMP		R0, #COLUMN_0_PRESSED
	BEQ		COLUMN_0_PRESSED

	CMP		R0, #COLUMN_1_PRESSED
	BEQ		COLUMN_1_PRESSED

	CMP		R0, #COLUMN_2_PRESSED
	BEQ		COLUMN_2_PRESSED

	CMP		R0, #COLUMN_3_PRESSED
	BEQ		COLUMN_3_PRESSED

COLUMN_0_PRESSED:
	MOV		R0, #COLUMN_0_PRESSED_BINARY
	B		JumpTableEnd

COLUMN_1_PRESSED:
	MOV		R0, #COLUMN_1_PRESSED_BINARY
	B		JumpTableEnd

COLUMN_2_PRESSED:
	MOV		R0, #COLUMN_2_PRESSED_BINARY
	B		JumpTableEnd

COLUMN_3_PRESSED:
	MOV		R0, #COLUMN_3_PRESSED_BINARY
	B		JumpTableEnd

JumpTableEnd:
	
	LSL		R7, #EVENT_INFO_SEGMENT_BITS ;we don't need R7 after this
	ORR		R0, R7 ;merge with current row

	LSL		R0, #EVENT_INFO_SEGMENT_BITS	
	ORR		R0, #EVENT_KEYDOWN

	;no need to save R0, R1, R2, R3, because we're not using them in the rest of the function
	BL		EnqueueEvent
	B		DebounceEnd

DebounceCounterNegative:
	MOV		R8, #0 ;reset counter to zero so we don't overflow in the negatives
	;B		DebounceEnd

DebounceEnd:
	STRB	R8, [R5] ;store new DebounceCounter
	B		KeypadScanAndDebounceEnd

ResetDebounce:
	STRB	R0, [R6] ; update PrevState

	;reset Debounce Counter
	MOV		R8, #DEBOUNCE_TIME

	;if a key is pressed, decrement the debounce counter to start 
	; debouncing in the next call
	CMP		R0, #COLUMN_ALL_KEYS_UP ; or if (CurState == 1111)
	BEQ		ResetDebounceEnd
	;B		KeyPressed

KeyPressed:
	SUB		R8, #1

ResetDebounceEnd:
	STRB	R8, [R5] ;store new DebounceCounter
	;B		KeypadScanAndDebounceEnd

KeypadScanAndDebounceEnd:
    POP     {LR, R4, R5, R6, R7, R8}
    BX 	    LR
