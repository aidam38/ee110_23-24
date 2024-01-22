;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  serial.s                                  ;
;                              Serial Interface                              ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains functions for interfacing with periphreals using the 
; SPI interface.
; 
; This file defines functions:
;		SerialSendRdy - returns true if the serial interface is ready to send
;		SerialSendData - sends a byte of data over the serial interface
;		SerialGetRdy - returns true if the serial interface has data to read
;		SerialGetData - reads a byte of data from the serial interface
; 
; Revision History:



; local includes
	.include "../std.inc"
	.include "serial_symbols.inc"
	.include "../cc26x2r/gpio_reg.inc"
	.include "../cc26x2r/gpt_reg.inc"
	.include "../cc26x2r/event_reg.inc"
	.include "../cc26x2r/aux_reg.inc"
	.include "../cc26x2r/ssi_reg.inc"

; export functions to other files
	.def InitSerial
	.def SerialSendRdy
	.def SerialSendData
	.def SerialGetRdy
	.def SerialGetData



; InitSerial
;
; Description:          Initializes the serial interface.
;
; Arguments:            None.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
; 
; Revision History:	

InitSerial:
	PUSH	{LR}						; save return address and used registers

	; configure pins
	MOV32	R1, IOC_BASE_ADDR			; prepare IOC base address
	STREG	TX_PIN_CFG, R1, IOCFG_REG_SIZE * TX_PIN ; configure TX pin
	STREG	RX_PIN_CFG, R1, IOCFG_REG_SIZE * RX_PIN ; configure RX pin
	STREG	CLK_PIN_CFG, R1, IOCFG_REG_SIZE * CLK_PIN ; configure CLK pin
	STREG	CS_PIN_CFG, R1, IOCFG_REG_SIZE * CS_PIN ; configure CS pin

	; configure SSI module
	MOV32	R1, SSI_BASE_ADDR			; prepare SSI0 base address
	STREG	SSI_CR0, R1, CR0_OFFSET		; configure SSI0 CR0
	STREG	SSI_CPSR, R1, CPSR_OFFSET	; configure SSI0 CPSR
	STREG	SSI_CR1, R1, CR1_OFFSET		; configure SSI0 CR1

    POP     {LR}				        ; restore return address and used registers
    BX      LR                          ; return



; SerialSendRdy
;
; Description:          Returns true if the serial interface is ready to send.
;
; Arguments:            None.
; Return Values:        R0 = 1 if the serial interface is ready to send, 0 otherwise.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

SerialSendRdy:
	PUSH	{LR}						; save return address and used registers

	MOV32	R1, SSI_BASE_ADDR			; prepare SSI0 base address
	LDR		R0, [R1, #SR_OFFSET]		; load status register
	TST		R0, #SR_TNF_NOTFULL			; test if transmit FIFO is not full
	BEQ		SerialSendRdyExit			; if transmit FIFO is full, return false
	MOV		R0, #1						; otherwise, return true
SerialSendRdyExit:
	POP		{LR}						; restore return address and used registers
	BX		LR							; return



; SerialSendData
;
; Description:          Sends data over the serial interface.
;
; Arguments:            R0 = data to send.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

SerialSendData:
	PUSH	{LR, R4}					; save return address and used registers

	MOV		R4, R0						; save argument in variable

	BL		SerialSendRdy				; check if the serial interface is ready to send
	CMP		R0, #0						; if the serial interface is not ready to send
	BEQ		SerialSendDataExit			; return without sending data

	MOV32	R1, SSI_BASE_ADDR			; prepare SSI0 base address
	STR		R4, [R1, #DR_OFFSET]		; send data

SerialSendDataExit:
	POP		{LR, R4}					; restore return address and used registers
	BX		LR							; return



; SerialGetRdy
;
; Description:          Returns true if the serial interface has data to read.
;
; Arguments:            None.
; Return Values:        R0 = 1 if the serial interface has data to read, 0 otherwise.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

SerialGetRdy:
	PUSH	{LR}						; save return address and used registers

	MOV32	R1, SSI_BASE_ADDR			; prepare SSI0 base address
	LDR		R0, [R1, #SR_OFFSET]		; load status register
	TST		R0, #SR_RNE_NOTEMPTY		; test if receive FIFO is not empty
	BEQ		SerialGetRdyExit			; if receive FIFO is empty, return false
	MOV		R0, #1						; otherwise, return true
SerialGetRdyExit:
	POP		{LR}						; restore return address and used registers
	BX		LR							; return



; SerialGetData
;
; Description:          Reads data from the serial interface.
;
; Arguments:            None.
; Return Values:        R0 = data read from the serial interface.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

SerialGetData:
	PUSH	{LR}						; save return address and used registers

	BL		SerialGetRdy				; check if the serial interface has data to read
	CMP		R0, #0						; if the serial interface does not have data to read
	BEQ		SerialGetDataExit			; return without reading data

	MOV32	R1, SSI_BASE_ADDR			; prepare SSI0 base address
	LDR		R0, [R1, #DR_OFFSET]		; read data

SerialGetDataExit:
	POP		{LR}						; restore return address and used registers
	BX		LR							; return

