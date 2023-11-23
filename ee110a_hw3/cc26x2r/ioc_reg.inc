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
IO_NOPUPD .equ                0x3    << 13   ; no pull up or down
IO_INPUT .equ                 0x1 << 29   ; input pin

; common pin settings
IOCFG_GENERIC_INPUT .equ      IO_NOPUPD | IO_INPUT
IOCFG_GENERIC_OUTPUT .equ     IO_NOPUPD


; other constants
IOCFG_REG_SIZE .equ    0x4