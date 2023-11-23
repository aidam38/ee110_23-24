; testing lol

; local include files
    .include "test_symbols.inc"
    .include "constants.inc"
    .include "macros.inc"

    .include "cc26x2r/ioc_macros.inc"
    .include "cc26x2r/store_event_handler.inc"
    .include "cc26x2r/gpt_reg.inc"
    .include "cc26x2r/gpio_reg.inc"
    .include "cc26x2r/cpu_scs_reg.inc"

; import symbols from other files
    .ref PeriphPowerInit
    .ref GPIOClockInit
    .ref StackInit
    .ref GPTClockInit

    .ref LCDInit
    .ref Display


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .text

    .global ResetISR ; exposing the program entry-point
ResetISR:
main:
    BL          PeriphPowerInit           ; turn on peripheral power domain
    BL          GPIOClockInit             ; turn on GPIO clock
    BL          GPTClockInit              ; turn on GPT clock
    BL          StackInit                 ; initialize stack in SRAM

	;testing pins 16 and 17
	IOCInit
	IOCFG	0,  IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	1,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	2,  IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	3,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	4,  IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	5,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	6,  IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	7,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	8,  IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	9,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	10, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	11,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	12, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	13,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	14, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	15,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	16, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	17,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	18, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	19,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	20, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	21,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	22, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	23,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	24, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	25,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	26, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	27,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	28, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	29,	IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	30, IOCFG_GENERIC_OUTPUT_8mA
	IOCFG	31,	IOCFG_GENERIC_OUTPUT_8mA

	 ; enable output for control pins
    MOV32       R1, GPIO_BASE_ADDR
    MOV32       R0, 0xFFFFFFFF
    STR         R0, [R1, #GPIO_DOE_OFFSET]
    STR			R0, [R1, #GPIO_DOUT_OFFSET]

Loop:
	NOP
	B Loop
