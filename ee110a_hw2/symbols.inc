
    .include "cc26x2r/ioc_reg.inc"
    .include "cc26x2r/gpt_reg.inc"

LED_PIN .equ    6
LED_CFG .equ    IO_NOPUPD

ROWSEL_A_PIN .equ   8
ROWSEL_B_PIN .equ   9

ROWSEL_PIN_CFG .equ   0x0

COLUMN_0_PIN .equ     11
COLUMN_1_PIN .equ     12
COLUMN_2_PIN .equ     13
COLUMN_3_PIN .equ     14

COLUMN_PIN_CFG .equ IO_INPUT | IO_PU

GPT_CFG .equ    GPT_CFG_32BIT
GPT_CTL .equ    GPT_CTL_TAEN_ENABLED
GPT_IMR .equ    GPT_IMR_TATOIM_ENABLED
GPT_TAMR .equ   GPT_TXMR_PERIODIC | GPT_TXMR_TXCDIR_DOWN

GPT_TAILR .equ  47999
GPT_TAPR .equ   0x0

GPT0A_IRQ_NUMBER .equ   15
GPT0A_EXCEPTION_NUMBER .equ 31

FUNCTION_CALL_SUCCESS .equ #0
FUNCTION_CALL_FAIL .equ #-1