;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            keypad_symbols.inc                              ;
;                       Symbols for the keypad driver                        ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the symbols for the keypad driver.
; 
; Revision History:
;     11/7/23  Adam Krivka      initial revision


    .include "../cc26x2r/ioc_reg.inc"


ROWSEL_PIN_CFG .equ   0x0               ; default output pin
COLUMN_PIN_CFG .equ IO_INPUT | IO_PU    ; default input pin with pullup

; debouncing
DEBOUNCE_TIME .equ              20      ; number of timer interrupts a key
                                        ; must be pressed down

; keypad row and column definitions
CURRENT_ROW_MASK .equ           11b     ; mask for the current row (need just
                                        ; low two bits because 4 rows)
COLUMN_ALL_KEYS_UP .equ         1111b   ; all keys up in a row (4 columns)

; one-hot encoding to binary encoding of column keys pressed
COLUMN_0_PRESSED_ONE_HOT .equ   0111b   
COLUMN_1_PRESSED_ONE_HOT .equ   1011b
COLUMN_2_PRESSED_ONE_HOT .equ   1101b
COLUMN_3_PRESSED_ONE_HOT .equ   1110b

COLUMN_0_PRESSED_BINARY .equ    0x0
COLUMN_1_PRESSED_BINARY .equ    0x1
COLUMN_2_PRESSED_BINARY .equ    0x2
COLUMN_3_PRESSED_BINARY .equ    0x3

; event vector definitions 
EVENT_INFO_SEGMENT_BITS .equ    4       ; number of bits in the event info
EVENT_KEYDOWN .equ              0xD     ; keydown event bit sequence
