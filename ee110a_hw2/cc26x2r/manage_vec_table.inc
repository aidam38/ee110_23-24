	.include "cpu_scs_reg.inc"

StoreEventHandlerInit .MACRO
	MOVW	R1, #(CPU_SCS_BASE_ADDR & 0xffff)
	MOVT	R1, #((CPU_SCS_BASE_ADDR >> 16) & 0xffff)
	LDR		R1, [R1, #VTOR_OFFSET]
  .ENDM

StoreEventHandler .MACRO event_handler, except_num
	MOVW	R0, event_handler
	MOVT	R0, event_handler
	STR		R0, [R1, #(4 * except_num)]
  .ENDM
