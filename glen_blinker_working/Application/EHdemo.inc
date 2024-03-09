;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                  EHdemo.inc                                ;
;                              Event Handler Demo                            ;
;                                 Include File                               ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains the constants for the event handler demo program.
;
; Revision History:
;     2/17/22  Glen George      initial revision
;     3/10/22  Glen George      removed <Tab>s



; general constants

FALSE              .EQU    0            ;false
TRUE               .EQU    1            ;true - non-zero
BYTES_PER_WORD     .EQU    4            ;4 bytes in each word


; system constants

HANDLER_STACK_SIZE .EQU    128          ;interrupt handler stack size
PROCESS_STACK_SIZE .EQU    256          ;main loop stack size
TOTAL_STACK_SIZE   .EQU    PROCESS_STACK_SIZE + HANDLER_STACK_SIZE

VEC_TABLE_SIZE     .EQU    54           ;number of interrupts in table

GPT0A_EX_NUM       .EQU    31           ;exception number for GPT0 Timer A
GPT0A_IRQ_NUM      .EQU    GPT0A_EX_NUM - 16    ;GPT0 Timer A interrupt number


; I/O constants

REDLED_IO_BIT      .EQU    6            ;red LED on bit 6
GREENLED_IO_BIT    .EQU    7            ;green LED on bit 7


; events

TIMEOUT_EVENT      .EQU    1            ;timeout event


; timing constants

MS_PER_BLINK       .EQU    1000         ;milliseconds for each blink
LOOPS_PER_BLINK    .EQU    100000       ;number of loops for blinking LED
GPT0A_MODE         .EQU    0x00000002   ;set timer A mode
CLK_PER_MS         .EQU    48000        ;timer clocks per millisecond
