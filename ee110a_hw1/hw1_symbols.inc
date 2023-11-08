;
; EE 110 Homework #1-specific constants for TI Launchpad CC26X2R1 board
; Include File
;
; Revision History: 10/27/23 Adam Krivka initial revision


;Power Control Registers

PRCM_BASE_ADDR .equ 		0x40082000

PDCTL0_OFFSET .equ 			0x012c

PDCTL0_RFC_ON .equ 			1b
PDCTL0_SERIAL_ON .equ 		10b
PDCTL0_PERIPH_ON .equ 		100b

PDSTAT0_OFFSET .equ 		0x140

PDSTAT0_RFC_ON .equ 		1b
PDSTAT0_SERIAL_ON .equ 		10b
PDSTAT0_PERIPH_ON .equ 		100b

GPIOCLKGR_OFFSET .equ		0x48

GPIOCLKGR_CLK_EN .equ		1b

CLKLOADCTL_OFFSET .equ 		0x28

CLKLOADCTL_LOAD .equ		1b
CLKLOADCTL_LOAD_DONE .equ 	10b

;IOC Registers

IOC_BASE_ADDR .equ 			0x40081000

IOCFG6_OFFSET .equ			0x18
IOCFG7_OFFSET .equ			0x1C
IOCFG13_OFFSET .equ			0x34
IOCFG14_OFFSET .equ			0x38

IO_PD .equ					0x1 << 13
IO_PU .equ					0x2 << 13
IO_NOPUPD .equ				0x3	<< 13
IO_INPUT .equ				0x1 << 29

;GPIO Registers
GPIO_BASE_ADDR .equ			0x40022000

GPIO_DOUT_OFFSET	.equ			0x80
GPIO_DIN_OFFSET .equ				0xC0
GPIO_DOE_OFFSET .equ				0xD0

DOE_ENABLE_6_7 .equ			(11b << 6)
D_HIGH_6_7 .equ				(11b << 6)

BTN_TO_LED_BIT_DISTANCE .equ 7
