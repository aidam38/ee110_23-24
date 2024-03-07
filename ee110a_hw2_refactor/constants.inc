; General purpose symbol definitions
;
; This file contains general purpose symbol definitions that are used
; throughout the project.  This file is included in almost all other files.
;
; Revision History:  11/7/23 Adam Krivka    initial revision

BYTES_PER_WORD  .equ        4       ; number of bytes in a word
BITS_PER_WORD .equ          32      ; number of bits in a word
TIMES_FOUR_LEFT_SHIFT .equ  2       ; left shift by 2 is the same as multiplying by 4

FUNCTION_CALL_SUCCESS .equ  0       ; return value for successful function call
FUNCTION_CALL_FAIL .equ     -1      ; return value for failed function call
