;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                 ioc_reg.inc                                ;
;                            IO Control Constants                            ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the constants for the IOC registers for the TI
; CC2652 microcontroller.
;
; Revision History:
;     11/7/23  Adam Krivka      initial revision


; base address
IOC_BASE_ADDR .equ             0x40081000  ; base address of IOC registers


; register values
IO_PD .equ                    0x1 << 13   ; pull down
IO_PU .equ                    0x2 << 13   ; pull up
IO_NOPUPD .equ                0x3 << 13   ; no pull up or down
IO_INPUT .equ                 0x1 << 29   ; input pin

IO_PORT_ID_GPIO .equ			0x0			; General Purpose IO
IO_PORT_ID_AON .equ				0x7			; AON 32 KHz clock (SCLK_LF)
IO_PORT_ID_AUXIO .equ			0x8			; AUX IO
IO_PORT_ID_EVENT0 .equ			0x17		; PORT EVENT 0
IO_PORT_ID_EVENT1 .equ			0x18		; PORT EVENT 1
IO_PORT_ID_EVENT2 .equ			0x19		; PORT EVENT 2
IO_PORT_ID_EVENT3 .equ			0x1A		; PORT EVENT 3
IO_PORT_ID_EVENT4 .equ			0x1B		; PORT EVENT 4
IO_PORT_ID_EVENT5 .equ			0x1C		; PORT EVENT 5
IO_PORT_ID_EVENT6 .equ			0x1D		; PORT EVENT 6
IO_PORT_ID_EVENT7 .equ			0x1E		; PORT EVENT 7

; common pin settings
IOCFG_GENERIC_INPUT .equ      IO_NOPUPD | IO_INPUT
IOCFG_GENERIC_OUTPUT .equ     IO_NOPUPD


; other constants
IOCFG_REG_SIZE .equ    0x4