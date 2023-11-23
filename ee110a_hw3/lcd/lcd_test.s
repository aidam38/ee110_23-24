;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  lcd_test.s                                ;
;                           14-pin LCD I/O Test Code                         ; 
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file defines functions:
;   LCDTest
; 



; local includes
; none

; import functions from other files
	.ref Display
	.ref DisplayChar
	.ref ClearDisplay

; export symbols to other files
    .def LCDTestDisplay
    .def LCDTestDisplayChar


	.text
; Test Table
; 
; 

hello_world:   .cstring "Hello world"
adam_krivka:   .cstring "Adam Krivka"

; display test cases table
    .align 4
LCDTestDisplayTab:
    ;     row,  col
    ;     str address
    .half 0,    0
    .word hello_world

    .half 0,    5
    .word adam_krivka

EndLCDTestDisplayTab:


; DisplayChar test cases table
    .align 4
LCDTestDisplayCharTab:
    ;     row, col, character, padding
    .byte 0, 0, "a", 0 
    .byte 0, 5, "b", 0

EndLCDTestDisplayCharTab:



; LCDTestDisplay
;
; Description:          
;
; Arguments:            None.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    
; Stack Depth:          
; 
; Revision History:
;     

LCDTestDisplay:
    PUSH        {LR, R4, R5}
    ; main loop
    ADR         R4, LCDTestDisplayTab          ; load address of test table
    ADR         R5, EndLCDTestDisplayTab       ; load address of end of test table
LCDTestDisplayLoop:
    BL          ClearDisplay            ; clear display

    ; load test case
    LDRH        R0, [R4], #2            ; load row
    LDRH        R1, [R4], #2            ; load column
    LDR         R2, [R4], #4            ; load string address

    BL          Display                 ; display string

    CMP         R4, R5                  ; compare current address to end address
    BNE         LCDTestDisplayLoop             ; if not at end, loop
    ;B          LCDTestDisplayEnd

LCDTestDisplayEnd:
    POP         {LR, R4, R5}
    BX          LR



; LCDTestDisplayChar
;
; Description:          
;
; Arguments:            None.
; Return Values:        None.
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    
; Stack Depth:          
; 
; Revision History:
;     

LCDTestDisplayChar:
    PUSH        {LR, R4, R5}

    BL          ClearDisplay            ; clear display

    ; main loop
    ADR         R4, LCDTestDisplayCharTab          ; load address of test table
    ADR         R5, EndLCDTestDisplayCharTab       ; load address of end of test table
LCDTestDisplayCharLoop:
    ; load test case
    LDRB        R0, [R4], #1            ; load row
    LDRB        R1, [R4], #1            ; load column
    LDRB        R2, [R4], #2            ; load character

    BL          DisplayChar             ; display character

    CMP         R4, R5                  ; compare current address to end address
    BNE         LCDTestDisplayCharLoop             ; if not at end, loop
    ;B          LCDTestDisplayCharEnd

LCDTestDisplayCharEnd:
    POP         {LR, R4, R5}
    BX          LR
