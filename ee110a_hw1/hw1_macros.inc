;
; Macros forEE 110 Homework #1
; Include File
;
; Revision History: 10/27/23 Adam Krivka initial revision


MOV32 .macro reg, value
	MOVW reg, #(value & 0xFFFF)
	MOVT reg, #((value >> 16) & 0xFFFF)
  .endm
