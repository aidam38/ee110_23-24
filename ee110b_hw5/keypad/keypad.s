;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   keypad.s                                 ;
;                                4x4 Keypad Drive                            ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the code for the keypad driver. It is responsible for
; scanning the keypad and debouncing the keys. It is also responsible for
; generating events when keys are pressed and released.
;
; The functions implemented in this file are:
;   KeypadInit - initializes the keypad driver
;   KeypadScanAndDebounce - scans the keypad and debounces the keys
;
; The interface, expected to be implemented in a separate file for flexibility,
; is as follows:
;  SelectRow - selects a row of the keypad
;  ReadRow - read the currently selected row of the keypad
;  EnqueueEvent - enqueue an event
; 
; Revision History:
;     11/7/23  Adam Krivka      initial revision


; local include files
    .include "keypad_symbols.inc"
    .include "../std.inc"

; import functions defined in other files
    .ref EnqueueEvent
    .ref SelectRow
    .ref ReadRow

; export functions defined in this file
    .def KeypadInit
    .def KeypadScanAndDebounce


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MEMORY
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .data
    .align 8

; CurrentRow - the current row being scanned, 0-3 (in binary)
CurrentRow: .byte 0

; DebounceCounter - the current debounce counter (debounce time is usually
;                    less than 8 bits)
DebounceCounter: .byte 0

; PrevState - the previous state of one row of the keypad (one-hot encoding)
PrevState: .byte 0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .text

; KeypadInit
;
; Description:         Initializes the keypad driver by setting the current row to 0,
;                        the debounce counter to DEBOUNCE_TIME, and the previous state
;                        to 1111b (all keys up).
;
; Arguments:         None.
; Return Values:     None.
;
; Local Variables:     None.
; Shared Variables: None.
; Global Variables: None.
;
; Inputs:             None.
; Outputs:             None.
;
; Error Handling: None.
;
; Registers Changed: R0, R1
; Stack Depth:        1
; 
; Revision History:
;     11/7/23  Adam Krivka      initial revision

KeypadInit:
    PUSH    {LR}            ; save return address

    ; initialize CurrentRow to 0
    MOVA    R0, CurrentRow
    MOV        R1, #0
    STRB    R1, [R0]

    ; initialize DebounceCounter to DEBOUNCE_TIME
    MOVA    R0, DebounceCounter
    MOV        R1, #DEBOUNCE_TIME
    STRB    R1, [R0]

    ; initialize PrevState to 1111b
    MOVA    R0, PrevState
    MOV        R1, #1111b
    STRB    R1, [R0]

    POP     {LR}            ; restore return address
    BX    LR



; KeypadScanAndDebounce
;
; Description:         Scans the keypad and debounces the keys. If a key is pressed,
;                     for DEBOUNCE_TIME an event is generated and enqueued. The 
;                    format of the enqueued event vector is as follows:
;                         [ROW][COLUMN][EVENT_KEYDOWN]
;                    where
;                        ROW is the row of the key pressed (4-bit binary)
;                        COLUMN is the column of the key pressed (4-bit binary)
;                        EVENT_KEYDOWN is the event type (4-bit binary)
;
; Operation:         This function is called periodically by the timer. Each
;                     time it is called, it checks whether the debounce counter
;                     is not its maximum value (DEBOUNCE_TIME), which indicates
;                     that a key is being debounced, in which case it checks
;                     whether that key is still being pressed, and if yes decrements
;                     the debounce counter further, and if not resets it to 
;                     its maximum value. If the debounce counter is zero, it enqueus
;                     an event as described above. If no key is pressed, the curent
;                     row is incremented and the function returns.
;
; Arguments:        None.
; Return Values:     None.
;
; Local Variables:     
;         R4 = CurrentRow address
;         R5 = DebounceCounter address
;         R6 = PrevState addresss
;         R7 = CurrentRow value
;         R8 = DebounceCounter value
; Shared Variables: None.
; Global Variables: None.
;
; Inputs:             Keypad.
; Outputs:             Queueing to Event Queue.
;
; Error Handling:     None.
;
; Registers Changed: flags, R0, R1
; Stack Depth:        6
;
; Revision History:
;     11/7/23  Adam Krivka      initial revision

KeypadScanAndDebounce:
    PUSH    {LR, R4, R5, R6, R7, R8}        ; save return address and registers

    ; load CurrentRow address and value
    MOVA    R4, CurrentRow
    LDRB    R7, [R4]

    ; load DebounceCounter address and value
    MOVA    R5, DebounceCounter
    LDRB    R8, [R5]

    ; compare DebounceCounter to DEBOUNCE_TIME
    CMP        R8, #DEBOUNCE_TIME ; 
    BNE        Read                        ; if DebounceCounter != DEBOUNCE_TIME, 
                                        ; don't scan, skip to reading the current row
    ;B        Scan                         ; if DebounceCounter == DEBOUNCE_TIME), 
                                        ; scan the keypad (advance to the next row)



Scan:
    ; increment CurrentRow
    ADD     R7, #1
    AND        R7, #CURRENT_ROW_MASK         ; take just lower two bits
    STRB    R7, [R4]                     ; update CurrentRow in memory
    
    ; call SelectRow(CurrentRow)
    MOV        R0, R7                         ;prepare argument
    BL      SelectRow                     ;SelectRow(CurrentRow)



Read:
    ; call ReadRow() to get the current state of the row (one-hot encoding)
    BL      ReadRow                     ; returns CurState in R0
    ;B        Compare                        ; compare CurState to PrevState

Compare:
    ; load PrevState address and value
    MOVA    R6, PrevState
    LDRB    R1, [R6]

    ; if PrevState != CurState, reset debounce state
    CMP        R0, R1
    BNE        ResetDebounce

    ; if PrevState == CurState AND CurState == 1111, no key is pressed
    ; nor was it pressed last time, so we can just end the function
    CMP        R0, #COLUMN_ALL_KEYS_UP
    BEQ        KeypadScanAndDebounceEnd

    ; if PrevState == CurState AND CurState != 1111 (meaning key is continuously
    ; being pressed), debounce
    ;B        Debounce



; Debounce current keypress using DebounceCounter
Debounce:
    SUBS    R8, #1                         ; decrement DebounceCounter value

    ; if DebounceCounter < 0
    BMI        DebounceCounterNegative 

    ; if DebounceCounter > 0
    BNE        DebounceEnd 

    ; if DebounceCounter == 0
    ;B        DebounceCounterZero 

DebounceCounterZero:
    ; We have successfully registered a key press!
    ; In this section:
    ;  - prepare event vector by converting column one-hot encoding to binary,
    ;    merging with row information, and EVENT_KEYDOWN value
    ;  - call EnqueueEvent(event_vector)

    ; Use jump table convert from one-hot encoding to binary
JumpTable:
    CMP        R0, #COLUMN_0_PRESSED_ONE_HOT
    BEQ        COLUMN_0_PRESSED

    CMP        R0, #COLUMN_1_PRESSED_ONE_HOT
    BEQ        COLUMN_1_PRESSED

    CMP        R0, #COLUMN_2_PRESSED_ONE_HOT
    BEQ        COLUMN_2_PRESSED

    CMP        R0, #COLUMN_3_PRESSED_ONE_HOT
    BEQ        COLUMN_3_PRESSED

COLUMN_0_PRESSED:
    MOV        R0, #COLUMN_0_PRESSED_BINARY
    B        JumpTableEnd

COLUMN_1_PRESSED:
    MOV        R0, #COLUMN_1_PRESSED_BINARY
    B        JumpTableEnd

COLUMN_2_PRESSED:
    MOV        R0, #COLUMN_2_PRESSED_BINARY
    B        JumpTableEnd

COLUMN_3_PRESSED:
    MOV        R0, #COLUMN_3_PRESSED_BINARY
    B        JumpTableEnd

JumpTableEnd:

    ; merge with row information    
    LSL        R7, #EVENT_INFO_SEGMENT_BITS ; we don't need R7 after this, so we can cobble it
    ORR        R0, R7

    ; merge with EVENT_KEYDOWN
    LSL        R0, #EVENT_INFO_SEGMENT_BITS    
    ORR        R0, #EVENT_KEYDOWN

    ; Call Enqueue(EventVector) (R0)
    ; (no need to save R0, R1, R2, R3, because we're not using them 
    ;  in the rest of the function)
    BL        EnqueueEvent
    B        DebounceEnd

DebounceCounterNegative:
    ; We need to reset DebounceCounter to zero so we don't overflow in the negatives
    MOV        R8, #0 
    ;B        DebounceEnd

DebounceEnd:
    ; store new DebounceCounter value (always done regardless of DebounceCounter
    ; value)
    STRB    R8, [R5] 
    B        KeypadScanAndDebounceEnd        ; end function



; Reset DebounceCounter
ResetDebounce:
    STRB    R0, [R6]                         ; update PrevState, because in this 
                                            ; clause we know that PrevState != CurState

    ; Reset Debounce Counter
    MOV        R8, #DEBOUNCE_TIME

    ; If a key is pressed, decrement DebounceCounter to start 
    ; debouncing in the next call
    CMP        R0, #COLUMN_ALL_KEYS_UP
    BEQ        ResetDebounceEnd
    ;B        KeyPressed

KeyPressed:
    SUB        R8, #1

ResetDebounceEnd:
    STRB    R8, [R5]                         ; store new DebounceCounter value in memory
    ;B        KeypadScanAndDebounceEnd        ; end function

KeypadScanAndDebounceEnd:
    POP     {LR, R4, R5, R6, R7, R8}        ; restore return address and registers
    BX         LR
