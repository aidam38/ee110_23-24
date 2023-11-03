; MoveVecTable
;
;
; Description: This function moves the interrupt vector table from
; its current location to SRAM at the location VecTable.
;
; Operation: The function reads the current location of the vector
; table from the Vector Table Offset Register and copies
; the words from that location to VecTable. It then
; updates the Vector Table Offset Register with the new
; address of the vector table (VecTable).
;
; Arguments: None.
; Return Values: None.
;
; Local Variables: None.
; Shared Variables: None.
; Global Variables: None.
;
; Input: VTOR.
; Output: VTOR.
;
; Error Handling: None.
;
; Registers Changed: flags, R0, R1, R2, R3
; Stack Depth: 1 word
;
; Algorithms: None.
; Data Structures: None.
;
; Revision History: 11/03/21 Glen George initial revision

	.include "cpu_scs_reg.inc"

	.def MoveVecTable

MoveVecTable:
	PUSH {R4} ;store necessary changed registers
	;B MoveVecTableInit ;start doing the copy\
	
MoveVecTableInit: ;setup to move the vector table
	MOVW R1, #(CPU_SCS_BASE_ADDR & 0xffff) ;get base for CPU SCS registers
	MOVT R1, #((CPU_SCS_BASE_ADDR >> 16) & 0xffff)
	LDR R0, [R1, #VTOR_OFFSET] ;get current vector table address
	MOVW R2, VecTable ;load address of new location
	MOVT R2, VecTable
	MOV R3, #VEC_TABLE_SIZE ;get the number of words to copy
	;B MoveVecCopyLoop ;now loop copying the table\
	
MoveVecCopyLoop: ;loop copying the vector table
	LDR R4, [R0], #BYTES_PER_WORD ;get value from original table
	STR R4, [R2], #BYTES_PER_WORD ;copy it to new table
	SUBS R3, #1 ;update copy count
	BNE MoveVecCopyLoop ;if not done, keep copying
	;B MoveVecCopyDone ;otherwise done copying
	
MoveVecCopyDone: ;done copying data, change VTOR
	MOVW R2, VecTable ;load address of new vector table
	MOVT R2, VecTable
	STR R2, [R1, #VTOR_OFFSET] ;and store it in VTOR
	;B MoveVecTableDone ;and all done
	
MoveVecTableDone: ;done moving the vector table
	POP {R4} ;restore registers and return
	BX LR

	.data

;the vector table in SRAM
	.align 512

VecTable:
	.SPACE VEC_TABLE_SIZE * BYTES_PER_WORD
