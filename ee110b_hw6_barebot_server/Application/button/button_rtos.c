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
*/



/* library includes */
#include  <ti/sysbios/hal/Hwi.h>
#include  <ti/sysbios/knl/Swi.h>
#include <button/button_rtos.h>
#include <button/button_rtos_intf.h>


/* local includes */


/* global variables */

/* swi interrupt task (needs to be global to be exposed to keypad.s)*/
Swi_Handle swiTask;


/* shared variables */

/* hwi interrupt task */
static Hwi_Struct hwiTask;

void ButtonSwi_wrapper() {
    ButtonDebounce();
}

void ButtonHwi_wrapper() {
    ButtonRTOSHwiHandler();
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
void ButtonInit_RTOS() {
    /* variables */
    Hwi_Params hwiParams;
    Swi_Params swiParams;

    /* set up and create Swi */
    Swi_Params_init(&swiParams);
    swiParams.priority = SWI_PRIORITY;
    swiTask = Swi_create(ButtonSwi_wrapper, &swiParams, NULL);

    /* set up and create Hwi */
    Hwi_Params_init(&hwiParams);
    hwiParams.eventId = TIMER_EXCEPTION_NUMBER;
    hwiParams.priority = HWI_PRIORITY;
    Hwi_construct(&hwiTask, TIMER_EXCEPTION_NUMBER, ButtonHwi_wrapper, &hwiParams, NULL);

    /* call assembly init function (inits GPIOs, variables, and starts timer) */
    ButtonInit();

    return;
}
