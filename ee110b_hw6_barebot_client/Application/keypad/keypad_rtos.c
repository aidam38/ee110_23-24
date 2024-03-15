/****************************************************************************/
/*                                                                          */
/*                                keypad_rtos.c                             */
/*                        Keypad RTOS C wrapper code                        */
/*                                                                          */
/****************************************************************************/

/* 4x4 Keypad RTOS C wrapper code. Functions included:
        KeypadInit_RTOS() - initialize the keypad using RTOS hardware and 
                            software interrupts

   Revision History:
       3/6/24  Adam Krivka      initial revision
       3/14/24 Adam Krivka      switched to using Clock
*/



/* library includes */
#include  "util.h"


/* local includes */
#include "keypad_rtos_intf.h"

/* declarations */
void KeypadInit();
void KeypadScanAndDebounce();

#define PERIOD_MILISECONDS 1


/* local variables */

/* clock task */
static Clock_Struct clock;
static Clock_Handle clockHandle;


void KeypadClockCB(){
    KeypadScanAndDebounce();
}

/*
   KeypadInit_RTOS()

   Description:     This function initializes the keypad using RTOS hardware and
                    software interrupts. It sets up the hardware and software 
                    interrupts and initializes the keypad hardware and software. 
                    It is called once at the start of the program.

   Operation:       Initializes the hardware and software interrupts, and then
                    calls the KeypadInit function which configures the GPIOs
                    , variables, and timer (it also starts it).

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None. Hwi and Swi functions are called with no error
                     checking, but they should not fail.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 3/6/24  Adam Krivka        initial revision
*/
void KeypadInit_RTOS() {
    /* variables */

    /* call assembly init function (inits GPIOs, variables) */
    KeypadInit();

    /* set up clock */
    clockHandle = Util_constructClock(&clock, KeypadClockCB, PERIOD_MILISECONDS, PERIOD_MILISECONDS, false, 0);

    /* start clock */
    Util_startClock(clockHandle);

    return;
}
