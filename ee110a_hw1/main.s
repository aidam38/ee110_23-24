;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            EE 110a - Homework #1                           ;
;                             Button-triggered LED                           ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This program configures a TI Launchpad CC26X2R1 board to
; turn on the red on-board LED (DIO6) when BTN-1 (DIO13) is pressed,
; and the green on-board LED (DIO7) when BTN-2 (DIO14) is pressed.
; The program first turns on the peripheral power domain on the board,
; then enables the GPIO clock, then configures the IO pins 6,7,13,14
; (including enabling output for 6,7), and runs a loop which copies
; the input register value of pins 13,14 to the output register value
; of pins 6,7. Note that we need to invert the value since the on-board
; buttons connect to ground, so we configure a pull-up resistor on pins
; 13,14, which means that each pin will be active when the button is not
; pressed, and inactive when it is pressed.
;
;
; Implementation Notes: R0 stores register-group memory base addresses, and
; R1 contains other miscellaneous values.
;
; Arguments: None.
; Return Values: None.
;
; Local Variables: None.
; Shared Variables: None.
; Global Variables: None.
;
; Input: None.
; Output: None.
;
; Error Handling: None.
;
; Registers Changed: flags, R0, R1
; Stack Depth: None (stack not used nor set up)
;
; Algorithms: None.
; Data Structures: None.
;
; Revision History: 10/27/23 Adam Krivka initial revision

; Include files
	.include "hw1_symbols.inc"
	.include "hw1_macros.inc"

; Exposing the program entry-point
	.global ResetISR

; Project-specific constants

LED_CFG .EQU	(IO_NOPUPD) 		; LED IO pin configuration
									; (identical for DIO6 and DIO7)
									; No pull-up/pull-down, output (by default)
BTN_CFG .EQU	(IO_PU | IO_INPUT)	; BUTTON IO pin configuartion
									; (identical for DIO13 and DIO14)
									; Pull-up resistor, input enabled, no
									; hysteresis, normal slew rate

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.text

ResetISR:
main:

;
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
	;B 		GPIOClockInit				;if yes, we have successfuly turned
										;turned peripheral power on, continue

;
; Initialize GPIO Clock (enable clock gate)
;

; Write to GPIOCLKGR and CLKLOADCTL (still in PCRM register group/domain)
GPIOClockInit:
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
	;B		IOCInit						;if yes, we have successfully turned
										;on the GPIO clock

;
; Initialize IO pins
;

; Write to IOCFG registers
IOCInit:
	MOV32	R0, IOC_BASE_ADDR			;put IOController registers base address
										;to R0

	MOV32	R1, LED_CFG					;put LED_CFG to R1 (see top of file for
										;description of LED_CFG)

	STR		R1, [R0, #IOCFG6_OFFSET]	;write R1 to IOCFG6
	STR		R1, [R0, #IOCFG7_OFFSET]	;write R2 to IOCFG7


	MOV32	R1, BTN_CFG					;put BTN_CFG to R1 (ditto)

	STR		R1, [R0, #IOCFG13_OFFSET]	;write R1 to IOCFG13
	STR		R1, [R0, #IOCFG14_OFFSET]	;write R1 to IOCFG14
	;B		GPIOInit

; Enable output for DIO6 and DIO7
GPIOInit:
	MOV32	R0, GPIO_BASE_ADDR			;put GPIO registers base addresss to R0
	MOV32	R1, DOE_ENABLE_6_7			;put DOE value for enabling output for
										;for DIO6 and DIO6 to R1
	STR		R1, [R0, #DOE_OFFSET]		;write R1 to DOE
	;B 		MainLoop

;
; Main program loop
; Turns on red LED, green LED when BTN-1, BTN-2 is pressed, respectively
;

MainLoop:
	;DIN and DOUT are in the same register group as DOE, so R0 is still a valid
	;base address
	LDR		R1, [R0, #DIN_OFFSET] 		;read DIN to R1 to get button input

	LSR		R1, #BTN_TO_LED_BIT_DISTANCE;right-shift R1 to move/map bits 14,13
										;to bits 7,6
	;(using the fact that the distance between 14 and 7 is the same as between
	;13 and 6)
	EOR		R1, #D_HIGH_6_7				;negate R1 because button pins go
										;inactive when button pressed

	STR		R1, [R0, #DOUT_OFFSET]		;write R1 to DOUT to output to LEDs
	B		MainLoop					;loop
