/****************************************************************************/
/*                                                                          */
/*                                button_rtos.c                             */
/*                        Button RTOS C wrapper code                        */
/*                                                                          */
/****************************************************************************/

/* 4x4 Button RTOS C wrapper code. Functions included:
        ButtonInit_RTOS() - initialize the button using RTOS hardware and
                            software interrupts

   Revision History:
       3/6/24  Adam Krivka      initial revision
       3/14/24 Adam Krivka      switched to using Clock
*/



/* library includes */
#include  "util.h"


/* local includes */
#include "button_rtos_intf.h"

/* declarations */
void ButtonInit();
void ButtonDebounce();

#define PERIOD_MILISECONDS 1


/* local variables */

/* clock task */
static Clock_Struct clock;
static Clock_Handle clockHandle;


void ButtonClockCB(){
    ButtonDebounce();
}

/*
   ButtonInit_RTOS()

   Description:     This function initializes the button using RTOS hardware and
                    software interrupts. It sets up the hardware and software 
                    interrupts and initializes the button hardware and software.
                    It is called once at the start of the program.

   Operation:       Initializes the hardware and software interrupts, and then
                    calls the ButtonInit function which configures the GPIOs
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
void ButtonInit_RTOS() {
    /* variables */

    /* call assembly init function (inits GPIOs, variables) */
    ButtonInit();

    /* set up clock */
    clockHandle = Util_constructClock(&clock, ButtonClockCB, PERIOD_MILISECONDS, PERIOD_MILISECONDS, false, 0);

    /* start clock */
    Util_startClock(clockHandle);

    return;
}
