    .include "cc26x2r/gpt_reg.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; INSTRUCTIONS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BUSY_FLAG_MASK .equ 0x1 << 7    ; the busy flag is at the 7th bit
SET_DDRAM_ADDR .equ 0x1 << 7    ; set DDRAM address instruction

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMERS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; select GPT1 timer for LCD
TIMER_BASE_ADDR .equ    GPT1_BASE_ADDR

; general settings
TIMER_ENABLE .equ          GPT_CTL_TAEN_ENABLED

; (interrupts disabled by default)

; timer for executing commands
CMDTIMER_CFG .equ       GPT_CFG_16BIT
CMDTIMER_TAMR .equ      GPT_TXMR_ONESHOT | GPT_TXMR_TXCDIR_UP

; 48 Mhz / 48 = 1 Mhz (1 us)
CMDTIMER_TAILR .equ    48
; 450 ns / (1 / 48 Mhz) = 21.6 ~ 22
CMDTIMER_TAMATCHR .equ 22
CMDTIMER_TAPR .equ     0

; timer for initializion
INITTIMER_CFG .equ      GPT_CFG_16BIT
INITTIMER_TAMR .equ     GPT_TXMR_ONESHOT | GPT_TXMR_TXCDIR_UP

; 3 * 2^4 = 48
INITTIMER_COUNT_PER_US .equ     3
INITTIMER_TAPR .equ             4