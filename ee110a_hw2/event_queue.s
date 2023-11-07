    .include "cc26x2r/cpu_scs_reg.inc"
    .include "macros.inc"

    .def EnqueueEvent

QUEUE_SIZE .equ     256

	.data
	.align 8

QueueBuffer: .space	QUEUE_SIZE

QueueIndex: .uword 0

	.text

EnqueueEvent:
	; R3 is index
	MOVA 	R2, QueueIndex 
	LDR		R3, [R2] 

	;check if stack is full
	CMP		R3, #QUEUE_SIZE
	BEQ		EnqueueEventFail

	; R1 is base
	MOVA 	R1, QueueBuffer 

	;write new value
	STR		R0, [R1, R3, LSL #2]

	;increment stack pointer
	ADD		R3, #1

	;store new index
	STR		R3, [R2]

	;B		EnqueueEventSuccess

EnqueueEventSuccess:
	MOV32	R0, #FUNCTION_CALL_SUCCESS
	B		EnqueueEventDone

EnqueueEventFail:
	MOV32	R0, #FUNCTION_CALL_FAIL
	;B		EnqueueEventDone

EnqueueEventDone:
	BX 		LR
