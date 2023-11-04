    .include "cc26x2r/cpu_scs_reg.inc"
    .include "macros.inc"

    .def EnqueueEvent

QUEUE_SIZE .EQU     256

	.data
	.align 8

QueueBuffer: .SPACE	QUEUE_SIZE

QueuePointer: .uword 0

	.text

EnqueueEvent:
	MOVA 	R1, QueueBuffer ; R1 is base
	MOVA 	R2, QueuePointer
	LDR		R3, [R2] ; R3 is offset
	STR		R0, [R1, R3]
	ADD		R3, #BYTES_PER_WORD
	STR		R3, [R2]
	BX 		LR
