;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            store_event_handler.inc                         ;
;            Macros for saving event handlers in the vector table            ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains macros:
;    StoreEventHandlerInit - initializes the vector table pointer
;    StoreEventHandler - stores an event handler in the vector table
; 
; Revision History:
;     11/7/23  Adam Krivka      initial revision

; local include files
    .include "cpu_scs_reg.inc"
    .include "../constants.inc"

;-----------------------------------------------------------------------------

; StoreEventHandlerInit
;
; Description:         This macro initializes the vector table pointer, which is used
;                      by the StoreEventHandler macro.
;
; Operation:           This macro loads the vector table pointer into R0.
; 
; Arguments:         none
;
; Registers Changed: R0
; Stack Depth:          0 words
;
; Author:            Adam Krivka
; Last Modified:    11/7/23

StoreEventHandlerInit .macro 
    MOVW    R0, #(SCS_BASE_ADDR & 0xffff)
    MOVT    R0, #((SCS_BASE_ADDR >> 16) & 0xffff)
    LDR        R0, [R0, #SCS_VTOR_OFFSET]
  .endm


; StoreEventHandler
;
; Description:         This macro stores an event handler in the vector table. The
;                      StoreEventHandlerInit macro must be called before this macro
;                      is called.
;
; Operation:           This macro loads the event handler address into R1 and then
;                    stores it in the vector table using an offset calculated
;                    from the exception number.
;
; Arguments:         event_handler - the address of the event handler
;                    except_num - the exception number
;
; Registers Changed: R1 (R0 just used)
; Stack Depth:          0 words
;
; Author:            Adam Krivka
; Last Modified:    11/7/23

StoreEventHandler .macro event_handler, except_num
    MOVW    R1, event_handler
    MOVT    R1, event_handler
    STR        R1, [R0, #(BYTES_PER_WORD * except_num)]
  .endm