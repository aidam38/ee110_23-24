;
; Macros forEE 110 Homework #1
; Include File
;
; Revision History: 10/27/23 Adam Krivka initial revision


MOV32 .MACRO reg, value
	MOVW reg, #(value & 0xFFFF)
	MOVT reg, #((value >> 16) & 0xFFFF)
  .ENDM

MOVA .MACRO reg, addr
	MOVW reg, addr
	MOVT reg, addr
  .ENDM
  
STREG .MACRO value, base, off
	MOVW R0, #(value & 0xFFFF)
	MOVT R0, #((value >> 16) & 0xFFFF)
	STR R0, [base, #off]
  .ENDM