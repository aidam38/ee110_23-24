;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                               ioc_macros.inc                               ;
;                         Macros for configuring IO pins                     ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains macros:
;    IOCInit - initializes the IOC module
;    IOCFG - configures a pin
; 
; Revision History:
;     11/7/23  Adam Krivka      initial revision


; local include files
    .include "cpu_scs_reg.inc"
    .include "../constants.inc"
    .include "ioc_reg.inc"

;-----------------------------------------------------------------------------

; IOCInit
; 
; Description:          Initializes the IOC module by loading the base address.
;
; Operation:            R0 = IOC_BASE_ADDR
;
; Arguments:            None
;
; Registers Changed:    R0
; Stack Depth:          0
;
; Author:               Adam Krivka
; Last Modified:        11/7/23

IOCInit .macro
    MOVW    R0, #(IOC_BASE_ADDR & 0xFFFF)
    MOVT    R0, #((IOC_BASE_ADDR >> 16) & 0xFFFF)
    MOV32   R0, IOC_BASE_ADDR
    .endm

; IOCFG
; 
; Description:          Configures a pin.
;
; Operation:            Writes the configuration to the IOCFG register for the
;                       specified pin (uses the pin number as an index).
;
; Arguments:            pin - the pin number
;                       cfg - the configuration (32-bit value)
;
; Registers Changed:    R1 (R0 just used)
; Stack Depth:          0
;
; Author:               Adam Krivka
; Last Modified:        11/7/23

IOCFG .macro pin, cfg
    MOVW    R1, #(cfg & 0xFFFF)
    MOVT    R1, #((cfg >> 16) & 0xFFFF)
    STR     R1, [R0, #(IOCFG_REGISTER_SIZE * pin)]
    .endm