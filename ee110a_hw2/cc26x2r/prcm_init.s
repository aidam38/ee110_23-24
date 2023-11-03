	.include "prcm_reg.inc"
	.include "../macros.inc"

	.def PDInit
	.def GPIOClockInit
	.def GPTClockInit

; Initialize periphreal power domain
;

; Write to PDCTL0
PDInit:
	MOV32 	R0, PCRM_BASE_ADDR			;put PCRM registers base address to R0
	MOV		R1, #PDCTL0_PERIPH_ON		;put PDCTL0 register value for turning
										;peripheral power on to R1
	STR 	R1, [R0, #PDCTL0_OFFSET]	;write R1 to PDCTL0
	;B		PDLoop

; Wait for PDSTAT0
PDLoop:
	LDR 	R1, [R0, #PDSTAT0_OFFSET]	;read PDCTL0 to R1
	CMP		R1, #PDSTAT0_PERIPH_ON		;check if R1 contains status value for
										;peripheral power on
	BNE		PDLoop						;^if not, loop/try again
	BX		LR


; Write to GPIOCLKGR and CLKLOADCTL (still in PCRM register group/domain)
GPIOClockInit:
	MOV32 	R0, PCRM_BASE_ADDR			;put PCRM registers base address to R0

	MOV		R1, #GPIOCLKGR_CLK_EN		;put CLK_EN (clock enable) value to R1
	STR		R1, [R0, #GPIOCLKGR_OFFSET]	;write R1 to GPIOCLKGR register
	;MOV	R1, #CLKLOADCTL_LOAD 		;put LOAD value to R1
										;(commented because same value as
										; GPIOCLKGR_CLK_EN)
	STR		R1, [R0, #CLKLOADCTL_OFFSET];write R1 to CLKLOADCTL
	;GPIOClockLoop

; Wait for CLKLOADCTL
GPIOClockLoop:
	LDR		R1, [R0, #CLKLOADCTL_OFFSET];read CLKLOADCTL to R1
	CMP		R1, #CLKLOADCTL_LOAD_DONE	;check if R1 has LOAD_DONE bit set
										;active
	BNE		GPIOClockLoop				;^if not, loop/try again
	BX		LR

	; Write to GPIOCLKGR and CLKLOADCTL (still in PCRM register group/domain)
GPTClockInit:
	MOV32 	R0, PCRM_BASE_ADDR			;put PCRM registers base address to R0

	MOV		R1, #GPTCLKGR_GPT0_ENABLE   ;put CLK_EN (clock enable) value to R1
	STR		R1, [R0, #GPTCLKGR_OFFSET]	;write R1 to GPIOCLKGR register

	;MOV		R1, #GPTCLKGS_GPT0_ENABLE   ;put CLK_EN (clock enable) value to R1
	;STR		R1, [R0, #GPTCLKGS_OFFSET]	;write R1 to GPIOCLKGR register

	;MOV		R1, #GPTCLKGDS_GPT0_ENABLE   ;put CLK_EN (clock enable) value to R1
	;STR		R1, [R0, #GPTCLKGDS_OFFSET]	;write R1 to GPIOCLKGR register

	MOV	R1, #CLKLOADCTL_LOAD 		;put LOAD value to R1
	STR		R1, [R0, #CLKLOADCTL_OFFSET];write R1 to CLKLOADCTL
	;GPIOClockLoop

; Wait for CLKLOADCTL
GPTClockLoop:
	LDR		R1, [R0, #CLKLOADCTL_OFFSET];read CLKLOADCTL to R1
	CMP		R1, #CLKLOADCTL_LOAD_DONE	;check if R1 has LOAD_DONE bit set
										;active
	BNE		GPTClockLoop				;^if not, loop/try again
	BX		LR
