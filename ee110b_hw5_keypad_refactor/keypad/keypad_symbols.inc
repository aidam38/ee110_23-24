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
;       11/7/23  Adam Krivka      initial revision
;       3/6/24   Adam Krivka      fixed formatting


; local includes
    .include "../cc26x2r/ioc_reg.inc"
    .include "../cc26x2r/gpt_reg.inc"
    .include "../cc26x2r/gpio_reg.inc"
    .include "../cc26x2r/cpu_scs_reg.inc"
    .include "../hw.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GPIO
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; pins for selecting rows
ROWSEL_A_PIN .equ       KEYPAD_ROWSEL_A_PIN
ROWSEL_B_PIN .equ       KEYPAD_ROWSEL_B_PIN
ROWSEL_CFG .equ         IOCFG_GENERIC_OUTPUT

; pins for selecting columns
COLUMN_0_PIN .equ       KEYPAD_COLUMN_0_PIN
COLUMN_1_PIN .equ       KEYPAD_COLUMN_1_PIN
COLUMN_2_PIN .equ       KEYPAD_COLUMN_2_PIN
COLUMN_3_PIN .equ       KEYPAD_COLUMN_3_PIN
COLUMN_CFG .equ         (IO_INPUT | IO_PU)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OTHER CONSTANS 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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
KEY_INFO_SEGMENT_BITS .equ      8       ; number of bits in the event info
