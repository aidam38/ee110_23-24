	.include "../macros.inc"

HANDLER_STACK_SIZE .EQU		128
PROCESS_STACK_SIZE .EQU		256
TOTAL_STACK_SIZE .EQU		PROCESS_STACK_SIZE + HANDLER_STACK_SIZE

	.def StackInit

; DATA
	.data
	.align 8

	.SPACE TOTAL_STACK_SIZE

TopOfStack:

; TEXT
	.text

StackInit:
	MOVA	R0, TopOfStack
	MSR		MSP, R0
	SUB		R0, R0, #HANDLER_STACK_SIZE
	MSR		PSP, R0
