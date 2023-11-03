;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            EE 110a - Homework #2                           ;
;                                   Keypad                                   ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; TODO
;
; Input: None.
; Output: None.
;
; Error Handling: None.
;
; Algorithms: None.
; Data Structures: None.
;
; Revision History: 10/27/23 Adam Krivka initial revision


; TODO
	.include "symbols.inc"
	.include "macros.inc"

	.include "cc26x2r/ioc_macros.inc"
	.include "cc26x2r/manage_vec_table.inc"
	.include "cc26x2r/gpt_reg.inc"
	.include "cc26x2r/gpio_reg.inc"
	.include "cc26x2r/cpu_scs_reg.inc"

	.ref PDInit
	.ref GPIOClockInit
	.ref StackInit
	.ref MoveVecTable
	.ref GPTClockInit

; Exposing the program entry-point
	.global ResetISR

	.UWORD EventHandler

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.text

EventHandler:
	SUBS	R4, #1
	;BNE		EventHandlerDone

	MOV32	R4, 0x1

	MOV32	R1, GPIO_BASE_ADDR
	STREG	0x1 << LED_PIN, R1, DOUTTGL_OFFSET

EventHandlerDone:
	MOV32	R1, GPT0_BASE_ADDR
	STREG	GPT_ICLR_TATOCINT_CLEAR, R1, GPT_ICLR_OFFSET

	BX		LR

ResetISR:
main:
	BL		PDInit ;turn on peripheral power domain
	BL		GPIOClockInit ;turn on gpio clock
	BL		StackInit
	BL		MoveVecTable

; configure IO pins
	IOCInit
	IOCFG 	LED_PIN, LED_CFG

	MOV32	R0, GPIO_BASE_ADDR			;put GPIO registers base addresss to R0
	MOV32	R1, 0x1 << LED_PIN			;put DOE value for enabling output for
										;for DIO6 and DIO6 to R1
	STR		R1, [R0, #DOE_OFFSET]		;write R1 to DOE

	MOV32	R1, 0x1 << LED_PIN
	STR		R1, [R0, #DOUTTGL_OFFSET]

; configure timers
	BL		GPTClockInit

	MOV32	R1, GPT0_BASE_ADDR
	STREG	GPT_CFG, R1, GPT_CFG_OFFSET
	STREG	GPT_IMR, R1, GPT_IMR_OFFSET
	STREG 	GPT_TAMR, R1, GPT_TAMR_OFFSET
	STREG	GPT_TAILR, R1, GPT_TAILR_OFFSET
	STREG	GPT_TAPR, R1, GPT_TAPR_OFFSET
	STREG	GPT_CTL, R1, GPT_CTL_OFFSET

	StoreEventHandlerInit
	StoreEventHandler	EventHandler, 31


	MOV32	R1, CPU_SCS_BASE_ADDR
	STREG	(0x1 << GPT0A_IRQ_NUMBER), R1, CPU_SCS_NVIC_ISER0

	MOV32	R4, 0x1

Loop:
	NOP
	B		Loop

