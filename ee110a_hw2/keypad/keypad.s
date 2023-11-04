    .include "keypad_symbols.inc"

    .ref EnqueueEvent
    .ref SelectRow
    .ref ReadRow

    .def KeypadInit
    .def KeypadScanAndDebounce

; Global variables
    .data

CurrentRow: .byte 0

DebounceBuffer: .SPACE NUM_COLS * DEBOUNCE_COUNTER_SIZE_BYTES

; Code
    .text

KeypadInit:
	BX	LR

KeypadScanAndDebounce:
    PUSH    {LR}

    ;increment CurrentRow
    MOVA    R0, CurrentRow
    ADD     R0, #1 ; we don't care about the higher bytes
    
    ;output CurrentRow
    BL      SelectRow
    BL      ReadRow

    ;for loop
    MOV     R1, #0

Loop:
    MOV     R2, R0
    LSL     R2, R1
    AND     R2, #1
    CMP     R2, #1

    ; init DebounceBuffer
    MOVA    R3, DebounceBuffer
    LDR     R4, [R3, R1, LSL #DEBOUNCE_COUNTER_SIZE_BYTES]

    BNE     ColumnIsUp
    ;B      ColumnIsDown

ColumnIsDown:
    SUBS    R4, #1
    BNE

CounterZero:
    MOV     R0, #0x69
    BL      EnqueueEvent
    B       CounterEnd

CounterNotZero:
    CMP     R4, #0
    BGEQ     CounterEnd

CounterLessThanZero:
    MOV     R4, #0

CounterEnd:
    B       EndLoop

ColumnIsUp:
    MOV     R4, #DEBOUNCE_COUNTER_INIT_VAL

EndLoop:
    STR     R4, [R3, R1, LSL #DEBOUNCE_COUNTER_SIZE_BYTES]

    ADD     R1, #1
    CMP     R1, #NUM_COLS
    BLE

    BL      EnqueueEvent
    POP     {LR}
    BX 	    LR
