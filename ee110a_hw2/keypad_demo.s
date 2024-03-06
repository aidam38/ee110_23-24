;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                            EE 110a - Homework #2                           ;
;                                   Keypad                                   ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file includes the entry point and main loop for the keypad demo program
; for EE 110a Homework #2.  The program first initializes the board (peripheral
; power, clocks, stack, vector table), then configures the GPIO pins according
; to the schematic/wiring, and finally sets up the GPT0A timer to trigger an
; interrupt every 1ms.  The interrupt handler then calls the KeypadScanAndDebounce
; function, which scans the keypad and debounces the keys. If a key is success-
; fully debounce, it is stored in the event queue.  This program relies solely
; on the interrupt event handler - the main loop does nothing.
;
; Revision History: 10/27/23 Adam Krivka initial revision


; local include files
    .include "keypad_demo_symbols.inc"
    .include "constants.inc"
    .include "macros.inc"

    .include "cc26x2r/ioc_macros.inc"
    .include "cc26x2r/store_event_handler.inc"
    .include "cc26x2r/gpt_reg.inc"
    .include "cc26x2r/gpio_reg.inc"
    .include "cc26x2r/cpu_scs_reg.inc"

; import symbols from other files
    .ref PeriphPowerInit
    .ref GPIOClockInit
    .ref StackInit
    .ref MoveVecTable
    .ref GPTClockInit
    .ref KeypadInit
    .ref KeypadScanAndDebounce



    .text
; GPT0EventHandler
; 
; Description: This function is the interrupt handler for the GPT0A timer.  
;               It calls the KeypadScanAndDebounce function, which scans the
;               keypad and debounces the keys. After KeypadScanAndDebounce, 
;               the event handler clears the interrupt and returns.
; 
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  timeout interrupt clear bit
; Global Variables:  None.
;
; Input:             None.
; Output:            None.
;
; Error Handling:    None.
;
; Algorithms:        None.
; Data Structures:   None.
;
; Registers Changed: None
; Stack Depth:       1 word
;
; Revision History:
;     11/7/23  Adam Krivka      initial revision

GPT0EventHandler:
    PUSH    {LR}                    ; save LR
    BL        KeypadScanAndDebounce

    ; clear the GPT0 time-out interrupt
    MOV32    R1, GPT0_BASE_ADDR
    STREG    GPT_ICLR_TATOCINT_CLEAR, R1, GPT_ICLR_OFFSET

    POP        {LR}                    ; restore LR
    BX        LR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MAIN CODE
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; exposing the program entry-point
    .global ResetISR
ResetISR:
main:
    BL        PeriphPowerInit         ; turn on peripheral power domain
    BL        GPIOClockInit             ; turn on GPIO clock
    BL        GPTClockInit            ; turn on GPT clock
    BL        StackInit                ; initialize stack in SRAM
    BL        MoveVecTable            ; initialize vector table in SRAM

    ; configure IO pins
    IOCInit
    IOCFG    ROWSEL_A_PIN, ROWSEL_PIN_CFG
    IOCFG    ROWSEL_B_PIN, ROWSEL_PIN_CFG
    IOCFG    COLUMN_0_PIN, COLUMN_PIN_CFG
    IOCFG    COLUMN_1_PIN, COLUMN_PIN_CFG
    IOCFG    COLUMN_2_PIN, COLUMN_PIN_CFG
    IOCFG    COLUMN_3_PIN, COLUMN_PIN_CFG

    ;enable output for pins 8,9
    MOV32    R1, GPIO_BASE_ADDR
    STREG    11b << ROWSEL_A_PIN, R1, GPIO_DOE_OFFSET

    ; configure timers
    MOV32    R1, GPT0_BASE_ADDR
    STREG    GPT_CFG, R1, GPT_CFG_OFFSET
    STREG    GPT_IMR, R1, GPT_IMR_OFFSET
    STREG    GPT_TAMR, R1, GPT_TAMR_OFFSET
    STREG    GPT_TAILR, R1, GPT_TAILR_OFFSET
    STREG    GPT_TAPR, R1, GPT_TAPR_OFFSET
    STREG    GPT_CTL, R1, GPT_CTL_OFFSET

    ; set GPT0 time-out interrupt handler
    StoreEventHandlerInit
    StoreEventHandler    GPT0EventHandler, GPT0A_EXCEPTION_NUMBER

    ; enable GPT0 time-out interrupt in the CPU
    MOV32    R1, SCS_BASE_ADDR
    STREG    (0x1 << GPT0A_IRQ_NUMBER), R1, SCS_NVIC_ISER0_OFFSET


    BL        KeypadInit                ; initialize keypad

; main loop - does nothing, everything happens in event handler
Loop:
    NOP
    B        Loop

