;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   asmutil                                  ;
;                          Assembly Utility Functions                        ;
;                              Event Handler Demo                            ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains utility function for the event handler demo that are
; written in assmebly.  These are just the functions from the assembly
; language version of the event handler demo.  The functions included are:
;    GPT0AEventHandler - event handler for GPTOA interrupt
;    InitClocks        - turn on the clock to the peripherals
;    InitGPIO          - initialize the I/O pins
;    InitGPT0          - initialize the timers (just GPT0A)
;    InitLEDs          - initialize the red and green LEDs
;    InitPower         - turn on power to the peripherals
;    SetGreenLED       - set the state of  the green LED
;    SetRedLED         - set the state of the red LED
;    ToggleGreenLED    - toggle the green LED
;    ToggleRedLED      - toggle the red LED
;
;
; Revision History:
;     2/18/22  Glen George      initial revision
;     3/7/22   Glen George      changed name to asmutil.s
;     3/7/22   Glen George      added SetRedLED and SetGreenLED functions.



; local include files
 .include  "CPUreg.inc"
 .include  "GPIOreg.inc"
 .include  "IOCreg.inc"
 .include  "GPTreg.inc"
 .include  "EHdemo.inc"
 .include  "macros.inc"




        .text



; GPT0AEventHandler
;
; Description:       This procedure is the event handler for the timer
;                    interrupt.  It posts a red LED event.
;
; Operation:         Posts a TIMEOUT_EVENT to the redLEDEvent event and
;                    returns.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
; Global Variables:  redLEDEvent - posts a TIMEOUT_EVENT to this event.
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
; Stack Depth:       5+ words
;
; Revision History:  02/18/22   Glen George      initial revision

GPT0AEventHandler:
        .def    GPT0AEventHandler


        ;external references
        .ref    redLEDEvent                     ;the event to post to
        .ref    ti_sysbios_knl_Event_post       ;the posting function



        PUSH    {R0 - R3, R9, R12, LR}  ;save the registers
                                        ;   don't know what Event_post trashes


SendTimeoutEvent:                       ;send the timeout event
        MOVA    R1, redLEDEvent         ;get the event handle
        LDR     R0, [R1]
        MOV32   R1, TIMEOUT_EVENT       ;get the event to post and post it
        BL      ti_sysbios_knl_Event_post
        ;B      ResetInt                ;and reset the interrupt


ResetInt:                               ;reset interrupt bit for GPT0A
        MOV32   R1, GPT0_BASE_ADDR      ;get base address
        STREG   GPT_IRQ_TATO, R1, GPT_ICLR_OFF ;clear the interrupt
        ;B      DoneInterrupt           ;all done with interrupts


DoneInterrupt:                          ;done with interrupt
        POP     {R0 - R3, R9, R12, LR}  ;restore registers
        BX      LR                      ;return from interrupt




; InitPower
;
; Description:       Turn on the power to the peripherals. 
;
; Operation:         Setup PRCM registers to turn on power to the peripherals.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  02/17/22   Glen George      initial revision

InitPower:
        .def    InitPower


        MOV32   R1, PRCM_BASE_ADDR              ;get base for power registers

        STREG   PD_PERIPH_EN, R1, PDCTL0_OFF    ;turn on peripheral power

WaitPowerOn:                                    ;wait for power on
        LDR     R0, [R1, #PDSTAT0_OFF]          ;get power status
        ANDS    R0, #PD_PERIPH_STAT             ;check if power is on
        BEQ     WaitPowerOn                     ;if not, keep checking
        ;BNE    DonePeriphPower                 ;otherwise done


DonePeriphPower:                                ;done turning on peripherals
        BX      LR




; InitClocks
;
; Description:       Turn on the clock to the peripherals. 
;
; Operation:         Setup PRCM registers to turn on clock to the peripherals.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  02/17/22   Glen George      initial revision

InitClocks:
        .def    InitClocks


        MOV32   R1, PRCM_BASE_ADDR              ;get base for power registers

        STREG   GPIOCLK_EN, R1, GPIOCLKGR_OFF   ;turn on GPIO clocks
        STREG   GPT0CLK_EN, R1, GPTCLKGR_OFF    ;turn on Timer 0 clocks
        STREG   GPTCLKDIV_1, R1, GPTCLKDIV_OFF  ;timers get system clock

        STREG   CLKLOADCTL_LD, R1, CLKLOADCTL_OFF  ;load clock settings

WaitClocksLoaded:                               ;wait for clocks to be loaded
        LDR     R0, [R1, #CLKLOADCTL_OFF]       ;get clock status
        ANDS    R0, #CLKLOADCTL_STAT            ;check if clocks are on
        BEQ     WaitClocksLoaded                ;if not, keep checking
        ;BNE    DoneClockSetup                  ;otherwise done


DoneClockSetup:                                 ;done setting up clock
        BX      LR




; InitGPIO
;
; Description:       Initialize the I/O pins for the LEDs.
;
; Operation:         Setup GPIO pins 6 and 7 to be 4 mA outputs for the LEDs.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  02/17/22   Glen George      initial revision

InitGPIO:
        .def    InitGPIO

                                        ;configure red and green LED outputs
        MOV32   R1, IOC_BASE_ADDR       ;get base for I/O control registers
        MOV32   R0, IOCFG_GEN_DOUT_4MA  ;setup for general 4 mA outputs
        STR     R0, [R1, #IOCFG6]   ;write configuration for red LED I/O
        STR     R0, [R1, #IOCFG7]   ;write configuration for green LED I/O

                                        ;enable outputs for LEDs
        MOV32   R1, GPIO_BASE_ADDR      ;get base for GPIO registers
        STREG   ((1 << REDLED_IO_BIT) | (1 << GREENLED_IO_BIT)), R1, GPIO_DOE31_0_OFF


        BX      LR                      ;done so return




; InitGPT0
;
; Description:       This function initializes GPT0.  It sets up the timer to
;                    generate interrupts every MS_PER_BLINK milliseconds.
;
; Operation:         The appropriate values are written to the timer control
;                    registers, including enabling interrupts.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  02/17/22   Glen George      initial revision

InitGPT0:
        .def    InitGPT0


GPT0AConfig:            ;configure timer 0A as a down counter generating
                        ;   interrupts every MS_PER_BLINK milliseconds

        MOV32   R1, GPT0_BASE_ADDR              ;get GPT0 base address
        STREG   GPT_CFG_32x1, R1, GPT_CFG_OFF   ;setup as a 32-bit timer
        STREG   GPT_CTL_TAEN, R1, GPT_CTL_OFF   ;enable timer A
        STREG   GPT_IRQ_TATO, R1, GPT_IMR_OFF   ;enable timer A timeout ints
        STREG   GPT0A_MODE, R1, GPT_TAMR_OFF    ;set timer A mode
                                                ;set 32-bit timer count
        STREG   (MS_PER_BLINK * CLK_PER_MS), R1, GPT_TAILR_OFF


        BX      LR                              ;done so return




; InitLEDs
;
; Description:       This function initializes the red and green LEDs.  The
;                    red LED is turned on and the green LED is turned off.
;
; Operation:         The output pin for the red LED is set to turn it on and
;                    the output pin for the green LED is cleared to turn it
;                    off.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  02/18/22   Glen George      initial revision

InitLEDs:
        .def    InitLEDs


        MOV32   R1, GPIO_BASE_ADDR      ;get base for GPIO registers

        ;turn on the red LED
        STREG   (1 << REDLED_IO_BIT), R1, GPIO_DSET31_0_OFF

        ;turn off the green LED
        STREG   (1 << GREENLED_IO_BIT), R1, GPIO_DCLR31_0_OFF


        BX      LR                              ;done so return




; SetGreenLED
;
; Description:       This function sets the state of the green LED.  It turns
;                    the LED on or off based on the passed argument.
;
; Operation:         The output pin for the green LED is set if the passed
;                    argument is non-zero and cleared if it is zero.
;
; Arguments:         R0 - new state of the LED, non-zero to turn on the LED
;                         and zero to turn off the LED.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  03/07/22   Glen George      initial revision

SetGreenLED:
        .def    SetGreenLED


        MOV32   R1, GPIO_BASE_ADDR      ;get base for GPIO registers

        ;check whether want the LED on or off
        ORRS    R0, R0
        BEQ     SetGreenLEDOff          ;turn off if argument is zero
        ;BNE    SetGreenLEDOn           ;turn on if argument is non-zero

SetGreenLEDOn:                          ;turn the green LED on
        STREG   (1 << GREENLED_IO_BIT), R1, GPIO_DSET31_0_OFF
        B       SetGreenLEDDone         ;and done

SetGreenLEDOff:                         ;turn the green LED off
        STREG   (1 << GREENLED_IO_BIT), R1, GPIO_DCLR31_0_OFF
        ;B      SetGreenLEDDone         ;and done

SetGreenLEDDone:
        BX      LR                      ;done so return




; SetRedLED
;
; Description:       This function sets the state of the red LED.  It turns
;                    the LED on or off based on the passed argument.
;
; Operation:         The output pin for the red LED is set if the passed
;                    argument is non-zero and cleared if it is zero.
;
; Arguments:         R0 - new state of the LED, non-zero to turn on the LED
;                         and zero to turn off the LED.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  03/07/22   Glen George      initial revision

SetRedLED:
        .def    SetRedLED


        MOV32   R1, GPIO_BASE_ADDR      ;get base for GPIO registers

        ;check whether want the LED on or off
        ORRS    R0, R0
        BEQ     SetRedLEDOff            ;turn off if argument is zero
        ;BNE    SetRedLEDOn             ;turn on if argument is non-zero

SetRedLEDOn:                            ;turn the red LED on
        STREG   (1 << REDLED_IO_BIT), R1, GPIO_DSET31_0_OFF
        B       SetRedLEDDone           ;and done

SetRedLEDOff:                           ;turn the red LED off
        STREG   (1 << REDLED_IO_BIT), R1, GPIO_DCLR31_0_OFF
        ;B      SetRedLEDDone           ;and done

SetRedLEDDone:
        BX      LR                      ;done so return




; ToggleGreenLED
;
; Description:       This function toggles the green LED.
;
; Operation:         The output pin for the green LED is toggled.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  02/18/22   Glen George      initial revision

ToggleGreenLED:
        .def    ToggleGreenLED


        MOV32   R1, GPIO_BASE_ADDR      ;get base for GPIO registers

        ;toggle the green LED
        STREG   (1 << GREENLED_IO_BIT), R1, GPIO_DTGL31_0_OFF


        BX      LR                      ;done so return




; ToggleRedLED
;
; Description:       This function toggles the red LED.
;
; Operation:         The output pin for the red LED is toggled.
;
; Arguments:         None.
; Return Value:      None.
;
; Local Variables:   None.
; Shared Variables:  None.
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
; Registers Changed: R0, R1
; Stack Depth:       0 words
;
; Revision History:  02/18/22   Glen George      initial revision

ToggleRedLED:
        .def    ToggleRedLED


        MOV32   R1, GPIO_BASE_ADDR      ;get base for GPIO registers

        ;toggle the red LED
        STREG   (1 << REDLED_IO_BIT), R1, GPIO_DTGL31_0_OFF


        BX      LR                      ;done so return


        .end
